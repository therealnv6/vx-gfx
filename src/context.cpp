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

    bool is_device_suitable(std::optional<vk::PhysicalDevice> device, vk::SurfaceKHR surface, const vk::QueueFlagBits flag_bits)
    {
        return gfx::find_queue_families(device, surface, flag_bits).is_complete();
    }

    void context::init_vulkan()
    {
        vk::ApplicationInfo app_info(this->window_name.c_str(),
            VK_MAKE_VERSION(1, 0, 0), "No Engine",
            VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);

        std::vector<const char *> validationLayers;

        vk::InstanceCreateInfo create_info({}, &app_info, (uint32_t) validationLayers.size());
        vk::UniqueInstance instance = vk::createInstanceUnique(create_info);

        std::vector<vk::PhysicalDevice> devices = instance->enumeratePhysicalDevices();
        std::optional<vk::PhysicalDevice> physical_device = nullptr;

        for (const auto &device : devices)
        {
            if (this->device_suitable(device))
            {
                physical_device = device;
                break;
            }
        }

        if (!physical_device.has_value())
        {
            throw std::runtime_error("failed to find a suitable device.");
        }

        vk::DeviceCreateInfo device_create_info({}, 0, nullptr, 0, nullptr);
        vk::UniqueDevice device = physical_device->createDeviceUnique(device_create_info);

        gfx::queue_family_indices indices = gfx::find_queue_families(
            physical_device, this->surface, this->flag_bits);

        auto queue_priority = 1.0f;
        vk::DeviceQueueCreateInfo queue_create_info({}, indices.graphics_family.value(), 1);
        queue_create_info.pQueuePriorities = &queue_priority;

        vk::PhysicalDeviceFeatures features {};
        vk::DeviceCreateInfo physical_create_info(
            vk::DeviceCreateFlags(), // flags
            1, // queueCreateInfoCount
            &queue_create_info, // pQueueCreateInfos
            static_cast<uint32_t>(validationLayers.size()), // enabledLayerCount
            nullptr, // ppEnabledLayerNames
            0, // enabledExtensionCount
            validationLayers.data(), // ppEnabledExtensionNames
            &features // pEnabledFeatures
        );

        vk::UniqueDevice unique_device = physical_device->createDeviceUnique(physical_create_info);
        vk::Queue queue = unique_device->getQueue(indices.graphics_family.value(), 0, this->graphics_queue);

        // set graphics_queue to the queue retrieved from the "logical device"
        this->graphics_queue = queue;
    }

    void context::create_surface()
    {
        auto surface = static_cast<VkSurfaceKHR>(this->surface);

        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
    }

}