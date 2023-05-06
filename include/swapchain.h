#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <device.h>
#include <functional>
#include <global.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace gfx
{
    // A structure that holds the swapchain's support details.
    struct swapchain_support_details
    {
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

        vk::SwapchainKHR chain; // The Vulkan swapchain object.
        vk::Format image_format; // The format of the swapchain's images.
        vk::Extent2D extent; // The extent of the swapchain's images.

        // Function for choosing a swap surface format.
        std::function<gfx::surface_format(gfx::surface_formats &available_formats)> choose_swap_surface = [](gfx::surface_formats &available_formats)
        {
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
        std::function<gfx::present_mode(const gfx::present_modes &available_present_modes)> choose_present_mode = [](const gfx::present_modes &available_present_modes)
        {
            // Default present mode if no other is chosen.
            return gfx::present_mode::eImmediate;
        };

        // Initializes the swapchain object.
        void initialize(GLFWwindow *window, vk::SurfaceKHR &surface);
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

    private:
        gfx::device *device; // The device associated with the swapchain.
    };
}
