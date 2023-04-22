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

        vk::DeviceCreateInfo deviceCreateInfo({}, 0, nullptr, 0, nullptr);
        vk::UniqueDevice device = physical_device->createDeviceUnique(deviceCreateInfo);
    }
}