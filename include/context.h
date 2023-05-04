#pragma once
#define GLFW_INCLUDE_VULKAN

#include <map>
#include <optional>
#include <vector>

#include <device.h>
#include <swapchain.h>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace gfx
{
    // forward declarations.
    class render_pass; // renderpass.h
    class pipeline; // pipeline.h

    /**
     * [context] class will handle most vulkan-specific actions.
     * Things like device management, synchronisation (semaphores, fences), surface management, are expected
     * to be managed through this class.
     *
     * @see [render_pass.h->gfx->render_pass]
     * @see [pipeline.h->gfx->pipeline]
     * @see [swapchain.h->gfx->swapchain]
     * @see [device.h->gfx->device]
     *      This handles all device initialisation.
     *      The main reason to keep this apart from the gfx::context class, is to
     *      be able to re-use the same device class for other surfaces/contexts.
     */
    class context
    {
    public:
        void init_swap_chain(swapchain swapchain);

    protected:
        vk::Queue graphics_queue;
        vk::Queue present_queue;

        vk::Instance instance;
        vk::SurfaceKHR surface;

        vk::CommandPool pool;

        std::vector<vk::CommandBuffer> command_buffers;
        std::vector<vk::Semaphore> image_available_semaphores;
        std::vector<vk::Semaphore> render_finished_semaphores;

        std::vector<vk::Fence> fences;
        uint32_t current_frame = 0;

        GLFWwindow *window;

        std::function<gfx::present_mode(const gfx::present_modes &available_present_modes)> choose_present_mode = [](const gfx::present_modes &available_present_modes)
        {
            return gfx::present_mode::eImmediate;
        };

        std::function<bool(vk::PhysicalDevice)> device_suitable = [](vk::PhysicalDevice device)
        {
            return true;
        };

    private:
        gfx::device *device;
        gfx::swapchain swapchain = nullptr;
        std::map<render_pass, std::vector<pipeline>> passes;

        bool has_validation_support();

        friend class swapchain;
        friend class render_pass;
        friend class pipeline;
    };
}