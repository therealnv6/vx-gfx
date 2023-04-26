#include <context.h>

namespace gfx
{
    context *context::current_context = nullptr;
    context::context(uint32_t width, uint32_t height, const char *name)
        : width { width }
        , height { height }
        , window_name { name }
    {
        gfx::context::current_context = this;
        this->init_window();
    }

    context::~context()
    {
        device->destroySwapchainKHR(this->swap_chain, nullptr);

        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(this->window);
        glfwTerminate();
    }

    void context::init_window()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        this->window = glfwCreateWindow(width, height, this->window_name.c_str(), nullptr, nullptr);
    }

    gfx::queue_family_indices find_queue_families(std::optional<vk::PhysicalDevice> device, vk::SurfaceKHR surface, const vk::QueueFlagBits flag_bits)
    {
        std::vector<vk::QueueFamilyProperties> queue_family_properties = device->getQueueFamilyProperties();
        gfx::queue_family_indices indices;

        uint32_t i = 0;

        for (const auto &queue_family : queue_family_properties)
        {
            if (indices.is_complete())
            {
                break;
            }

            if (queue_family.queueFlags & flag_bits)
            {
                indices.graphics_family = i;

                if (device->getSurfaceSupportKHR(i, surface))
                {
                    indices.present_family = i;
                }
            }
        }

        return indices;
    }

    gfx::swapchain_support_details query_swapchain_support(std::optional<vk::PhysicalDevice> device, vk::SurfaceKHR surface)
    {
        return gfx::swapchain_support_details {
            .capabilities = device->getSurfaceCapabilitiesKHR(surface),
            .formats = device->getSurfaceFormatsKHR(surface),
            .present_modes = device->getSurfacePresentModesKHR(surface),
        };
    }

    bool is_device_suitable(std::optional<vk::PhysicalDevice> device, vk::SurfaceKHR surface, const vk::QueueFlagBits flag_bits)
    {
        return gfx::find_queue_families(device, surface, flag_bits).is_complete() && gfx::query_swapchain_support(device, surface).is_complete();
    }

    void context::init_vulkan()
    {
        vk::ApplicationInfo app_info(this->window_name.c_str(),
            VK_MAKE_VERSION(1, 0, 0), "No Engine",
            VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);

        std::vector<std::string> validation_layers;
        vk::InstanceCreateInfo create_info({}, &app_info, (uint32_t) validation_layers.size());
        vk::UniqueInstance instance = vk::createInstanceUnique(create_info);

        std::vector<vk::PhysicalDevice> devices = instance->enumeratePhysicalDevices();

        vk::DeviceQueueCreateInfo queue_create_info({}, 0, 1);
        vk::PhysicalDeviceFeatures features {};

        // we have to create the surface before we instantiate the queues
        this->create_surface();

        for (const auto &device : devices)
        {
            if (this->device_suitable(device))
            {
                this->physical_device = device;

                gfx::queue_family_indices indices = find_queue_families(device, this->surface, this->flag_bits);

                // create the device with suitable physical device
                vk::DeviceCreateInfo device_create_info({}, 1, &queue_create_info);

                // enable the device extensions required
                device_create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
                device_create_info.ppEnabledExtensionNames = device_extensions.data();

                vk::UniqueDevice unique_device = device.createDeviceUnique(device_create_info);

                // set graphics_queue and present_queue to the queue retrieved from the "logical device"
                this->graphics_queue = unique_device->getQueue(indices.graphics_family.value(), 0);
                this->present_queue = unique_device->getQueue(indices.present_family.value(), 0);
                this->device = std::move(unique_device);

                break;
            }
        }

        this->create_swap_chain();
    }

    void context::create_surface()
    {
        auto surface = static_cast<VkSurfaceKHR>(this->surface);

        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void context::create_swap_chain()
    {
        gfx::swapchain_support_details details = query_swapchain_support(this->physical_device, surface);

        vk::SurfaceFormatKHR surface_format = choose_swap_surface(details.formats);
        vk::PresentModeKHR present_mode = choose_present_mode(details.present_modes);
        vk::Extent2D extent = choose_swap_extent(details.capabilities);

        uint32_t image_count = std::max(details.capabilities.minImageCount + 1, details.capabilities.maxImageCount);

        vk::SwapchainCreateInfoKHR create_info(vk::SwapchainCreateFlagBitsKHR::eMutableFormat, this->surface, image_count, surface_format.format, surface_format.colorSpace, extent, 1, vk::ImageUsageFlagBits::eColorAttachment);
        gfx::queue_family_indices indices = find_queue_families(this->physical_device, this->surface, this->flag_bits);
        uint32_t queue_family_indices[] = { indices.graphics_family.value(), indices.present_family.value() };

        if (indices.graphics_family != indices.present_family)
        {
            create_info.imageSharingMode = vk::SharingMode::eConcurrent;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = queue_family_indices;
        }
        else
        {
            create_info.imageSharingMode = vk::SharingMode::eExclusive;
        }

        create_info.imageExtent = extent;
        create_info.preTransform = details.capabilities.currentTransform;
        create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        create_info.presentMode = present_mode;
        create_info.clipped = true;

        this->swap_chain = device->createSwapchainKHR(create_info, nullptr);
        this->images = device->getSwapchainImagesKHR(this->swap_chain);

        this->swap_chain_image_format = surface_format.format;
        this->swap_chain_extent = extent;
    }

    vk::Extent2D context::choose_swap_extent(const vk::SurfaceCapabilitiesKHR &capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width;
            int height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }
}