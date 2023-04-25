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

        vk::DeviceQueueCreateInfo queue_create_info({}, 0, 1);
        vk::PhysicalDeviceFeatures features {};

        // we have to create the surface before we instantiate the queues
        this->create_surface();

        for (const auto &device : devices)
        {
            if (this->device_suitable(device))
            {
                gfx::queue_family_indices indices = find_queue_families(device, this->surface, this->flag_bits);

                // create the device with suitable physical device
                vk::DeviceCreateInfo device_create_info({}, 1, &queue_create_info);
                vk::UniqueDevice unique_device = device.createDeviceUnique(device_create_info);

                // set graphics_queue and present_queue to the queue retrieved from the "logical device"
                this->graphics_queue = unique_device->getQueue(indices.graphics_family.value(), 0);
                this->present_queue = unique_device->getQueue(indices.present_family.value(), 0);

                break;
            }
        }
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