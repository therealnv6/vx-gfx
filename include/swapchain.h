#pragma once

#define GLFW_INCLUDE_VULKAN

#include <functional>

#include <GLFW/glfw3.h>
#include <device.h>
#include <global.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace gfx
{
    struct swapchain_support_details
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> present_modes;

        bool is_complete()
        {
            return !this->formats.empty() && !this->present_modes.empty();
        }
    };

    gfx::swapchain_support_details query_swapchain_support(std::optional<vk::PhysicalDevice> device, vk::SurfaceKHR surface);

    class swapchain
    {
    public:
        std::vector<vk::Image> images;
        std::vector<vk::ImageView> image_views;

        vk::SwapchainKHR chain;
        vk::Format image_format;
        vk::Extent2D extent;

        std::function<gfx::surface_format(gfx::surface_formats &available_formats)> choose_swap_surface = [](gfx::surface_formats &available_formats)
        {
            for (const auto &available_format : available_formats)
            {
                if (available_format.format == vk::Format::eB8G8R8A8Srgb && available_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                {
                    return available_format;
                }
            }

            return available_formats[0];
        };

        // Function for choosing a present mode.
        // This function takes a vector of available present modes as input
        // and returns a chosen present mode.
        std::function<gfx::present_mode(const gfx::present_modes &available_present_modes)> choose_present_mode = [](const gfx::present_modes &available_present_modes)
        {
            // Default present mode if no other is chosen.
            return gfx::present_mode::eImmediate;
        };

        void initialize(GLFWwindow *window, vk::SurfaceKHR &surface);
        void create_swapchain(vk::SurfaceKHR &surface, gfx::swapchain_support_details details);

        vk::Extent2D choose_swap_extent(const vk::SurfaceCapabilitiesKHR &capabilities, GLFWwindow *window);

        swapchain(gfx::device *device)
            : device { device } {};

    private:
        gfx::device *device;
    };
}