#pragma once

#include <unordered_map>
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <device.h>
#include <functional>
#include <global.h>
#include <vulkan/vulkan.hpp>

namespace gfx
{
    // A structure that holds the swapchain's support details.
    struct swapchain_support_details {
        vk::SurfaceCapabilitiesKHR capabilities; // The surface's capabilities.
        std::vector<vk::SurfaceFormatKHR> formats; // The surface's available formats.
        std::vector<vk::PresentModeKHR> present_modes; // The surface's available present modes.

        // Returns true if the required details are complete.
        bool is_complete()
        {
            return !this->formats.empty() && !this->present_modes.empty();
        }
    };

    // Queries the swapchain's support details for a given device and surface.
    gfx::swapchain_support_details query_swapchain_support(std::optional<vk::PhysicalDevice> device, vk::SurfaceKHR surface);

    // forward declaration!
    class swapchain;
    /**
     * The `render_pass` class represents a Vulkan render pass object,
     * which defines the inputs, outputs, and dependencies of a rendering operation.
     *
     * It provides functionality to create and manage the render pass, including creating
     * framebuffers and cleaning up resources.
     *
     * @see `gfx::swapchain` - The swapchain class manages a Vulkan swapchain for presenting rendered images.
     * @see `vk::RenderPass` - The low-level Vulkan render pass which this object wraps around.
     */
    class render_pass
    {
    public:
        std::vector<vk::Framebuffer> framebuffers; // A vector of Vulkan framebuffer handles.
        vk::RenderPass pass;

        vk::SampleCountFlags samples = vk::SampleCountFlagBits::e1;
        vk::AttachmentStoreOp store_operation = vk::AttachmentStoreOp::eStore;
        vk::AttachmentLoadOp load_operation = vk::AttachmentLoadOp::eClear;

        vk::AttachmentLoadOp stencil_load_op = vk::AttachmentLoadOp::eDontCare;
        vk::AttachmentStoreOp stencil_store_op = vk::AttachmentStoreOp::eDontCare;

        vk::ImageLayout initial_layout = vk::ImageLayout::eUndefined;
        vk::ImageLayout final_layout = vk::ImageLayout::ePresentSrcKHR;

        void begin(vk::CommandBuffer *buffer, uint32_t index, vk::ClearValue clear);
        void end(vk::CommandBuffer *buffer);

        // This function creates the Vulkan render pass for the pipeline.
        void create_render_pass();

        // This function creates the Vulkan framebuffers for the pipeline.
        void create_frame_buffers();

        // This function cleans up resources used by the render pass and its framebuffers.
        void cleanup();

        // Constructor for the render_pass class.
        render_pass(gfx::swapchain *swapchain,
            vk::SampleCountFlags samples,
            vk::AttachmentStoreOp store_operation,
            vk::AttachmentLoadOp load_operation,
            vk::AttachmentLoadOp stencil_load_op,
            vk::AttachmentStoreOp stencil_store_op,
            vk::ImageLayout initial_layout,
            vk::ImageLayout final_layout);

    private:
        // The parent swapchain and device the render pass belongs to.
        gfx::swapchain *swapchain;
        gfx::device *device;
    };

    /**
     * Starts a new render pass with the specified configuration.
     *
     * @param swapchain - The swapchain object to create the render pass for.
     * @param samples - The number of samples used for multi-sampling.
     * @param store_operation - The store operation to use for the render pass attachment.
     * @param load_operation - The load operation to use for the render pass attachment.
     * @param stencil_load_op - The load operation to use for the stencil component of the render pass attachment.
     * @param stencil_store_op - The store operation to use for the stencil component of the render pass attachment.
     * @param initial_layout - The initial layout of the attachment.
     * @param final_layout - The final layout of the attachment.
     * @return A new render_pass object with the specified configuration.
     */
    render_pass start_render_pass(gfx::swapchain *swapchain,
        vk::SampleCountFlags samples = vk::SampleCountFlagBits::e1,
        vk::AttachmentStoreOp store_operation = vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp load_operation = vk::AttachmentLoadOp::eClear,
        vk::AttachmentLoadOp stencil_load_op = vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp stencil_store_op = vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout initial_layout = vk::ImageLayout::eUndefined,
        vk::ImageLayout final_layout = vk::ImageLayout::ePresentSrcKHR);

    /**
     * The `swapchain` class manages a Vulkan swapchain, which is responsible for presenting rendered images to a window surface.
     *
     * It provides functionality to create and manage the swapchain,
     including querying and selecting the optimal image format and presentation mode.
     *
     * @see `gfx::device` - The device class handles device initialization, and can be reused across multiple contexts.
     * @see `vk::SwapchainKHR` - The low-level vulkan swapchain which this object wraps around.
     */
    class swapchain
    {
    public:
        std::vector<vk::Image> images; // The swapchain's images.
        std::vector<vk::ImageView> image_views; // The swapchain's image views.

        std::unordered_map<std::string, render_pass> render_passes; // the vulkan render passes

        vk::SwapchainKHR chain; // The Vulkan swapchain object.
        vk::Format image_format; // The format of the swapchain's images.
        vk::Extent2D extent; // The extent of the swapchain's images.

        gfx::device *device; // The device associated with the swapchain.

        // Function for choosing a swap surface format.
        std::function<gfx::surface_format(gfx::surface_formats &available_formats)> choose_swap_surface = [](gfx::surface_formats &available_formats) {
            // Chooses the first available format that matches the desired format.
            for (const auto &available_format : available_formats)
            {
                if (available_format.format == vk::Format::eB8G8R8A8Srgb && available_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                {
                    return available_format;
                }
            }

            // Returns the first available format if no matching format is found.
            return available_formats[0];
        };

        // Function for choosing a present mode.
        std::function<gfx::present_mode(const gfx::present_modes &available_present_modes)> choose_present_mode = [](const gfx::present_modes &available_present_modes) {
            // Default present mode if no other is chosen.
            return gfx::present_mode::eImmediate;
        };

        // Initializes the swapchain object.
        void initialize(GLFWwindow *window, vk::SurfaceKHR &surface);

        void add_render_pass(std::string key, gfx::render_pass pass)
        {
            this->render_passes.try_emplace(key, pass);
        }

        void cleanup();

        // Creates the swapchain object.
        void create_swapchain(vk::SurfaceKHR &surface, gfx::swapchain_support_details details);

        void create_image_views();

        // Chooses the swap extent based on the surface's capabilities and the window's size.
        vk::Extent2D choose_swap_extent(const vk::SurfaceCapabilitiesKHR &capabilities, GLFWwindow *window);

        // Constructor for the swapchain class.
        swapchain(gfx::device *device)
            : device { device } {};

        ~swapchain();

    protected:
        friend class render_pass;
    };
}
