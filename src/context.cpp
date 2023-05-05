#include <context.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace gfx
{
    context::context()
    {
        spdlog::info("initializing gfx::context()");
        this->create_window();
        this->create_instance();
        this->create_surface();
        spdlog::info("initialized gfx::context()");
    }

    // Cleans up resources used by the context.
    void context::cleanup()
    {
        // we have to destroy this stuff before we destroy the actual instance.
        instance.destroySurfaceKHR(surface);
        instance.destroy(); // destroy Vulkan instance

        glfwDestroyWindow(window); // destroy GLFW window
        glfwTerminate(); // clean up GLFW
    }

    // Initializes the swap chain for the context.
    void context::init_swap_chain(gfx::swapchain swapchain)
    {
        this->swapchain = swapchain;
    }

    // Creates a GLFW window for the context.
    void context::create_window()
    {
        spdlog::info("creating GLFW window");
        glfwInit(); // initialize GLFW

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // tell GLFW not to create an OpenGL context
        glfwWindowHint(GLFW_RESIZABLE, GLFW_NO_API); // make the window non-resizable

        window = glfwCreateWindow(
            WIDTH, HEIGHT, // window size
            "vuxol", // window title
            nullptr, // monitor to use for fullscreen mode
            nullptr // window to share resources with (none)
        );
    }

    // Creates a Vulkan instance for the context.
    void context::create_instance()
    {
        spdlog::info("creating vulkan window");

        uint32_t glfw_extension_count = 0;
        const char **glfw_extensions;

        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

        vk::ApplicationInfo app_info("vuxol", // application name
            VK_MAKE_VERSION(1, 0, 0), // application version
            "furry engine", // engine name
            VK_MAKE_VERSION(1, 0, 0), // engine version
            VK_API_VERSION_1_0); // Vulkan API version

        vk::InstanceCreateInfo instance_info({},
            &app_info, // pointer to application info
            0, nullptr, // no validation layers
            glfw_extension_count, glfw_extensions); // GLFW extensions

        this->instance = vk::createInstance(instance_info); // create the Vulkan instance
    }

    void context::create_surface()
    {
        VkSurfaceKHR surface;

        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }

        this->surface = surface;
    }
}
