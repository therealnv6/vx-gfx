#pragma once
#define GLFW_INCLUDE_VULKAN

#include <map>
#include <optional>
#include <vector>

#include <commands.h>
#include <device.h>
#include <swapchain.h>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace gfx
{
    /**
     * [context] class will handle most Vulkan-specific actions.
     * It manages device, surface, synchronization, and swapchain creation.
     *
     * @see [render_pass.h->gfx->render_pass] - Render pass header file.
     * @see [pipeline.h->gfx->pipeline] - Pipeline header file.
     * @see [swapchain.h->gfx->swapchain] - Swapchain header file.
     * @see [device.h->gfx->device] - Device header file.
     *      This class handles all device initialization.
     *      The main reason to keep this separate from the gfx::context class is to
     *      allow reusing the same device class for other surfaces/contexts.
     */
    class context
    {
    public:
        context();
        ~context()
        {
            this->cleanup();
        };
        // Initializes the swapchain for this context.
        void init_swap_chain(swapchain *swapchain);

        // Vulkan instance and surface.
        vk::Instance instance;
        vk::SurfaceKHR surface;

        // Pointer to device and swapchain objects.
        gfx::device *device = nullptr;
        gfx::swapchain *swapchain = nullptr;
        gfx::commands *commands = nullptr;

    protected:
        vk::DebugUtilsMessengerEXT debugger;

        // GLFW window.
        GLFWwindow *window;

    private:
        const uint32_t WIDTH = 800;
        const uint32_t HEIGHT = 600;

        // Function for checking if validation support is available.
        bool has_validation_support();
        void create_window();
        void create_instance();
        void create_surface();

        void cleanup();

        std::vector<const char *> get_required_extensions();

        // Friend classes for accessing private members.
        friend class swapchain;
        friend class render_pass;
        friend class pipeline;
    };
}
