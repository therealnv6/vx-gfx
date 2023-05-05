#pragma once

#include <functional>

#include <device.h>
#include <global.h>
#include <vulkan/vulkan.hpp>

namespace gfx
{
    // forward declaration. found in context.h
    class context;

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

    class swapchain
    {
    public:
        std::vector<vk::Image> images;
        std::vector<vk::ImageView> image_views;

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

        swapchain(gfx::device *device)
            : device { device } {};

    private:
        gfx::device *device;
    };
}