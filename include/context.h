#pragma once

#define GLFW_INCLUDE_VULKAN

#include <cstdlib>
#include <functional>
#include <iostream>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp> // modern c++!

#include <limits>
#include <optional>
#include <ranges>
#include <set>

namespace gfx
{
    static const std::vector<const char *> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    typedef vk::PresentModeKHR present_mode;
    typedef vk::SurfaceFormatKHR surface_format;

    typedef std::vector<present_mode> present_modes;
    typedef std::vector<surface_format> surface_formats;

    struct queue_family_indices
    {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        bool is_complete()
        {
            return graphics_family.has_value() && present_family.has_value();
        }
    };

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

    class context
    {
    public:
        vk::QueueFlagBits flag_bits = vk::QueueFlagBits::eGraphics;

        vk::Queue present_queue;
        vk::Queue graphics_queue;

        GLFWwindow *window;

        std::function<bool(vk::PhysicalDevice)> device_suitable = [](vk::PhysicalDevice device)
        {
            return true;
        };

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

        std::function<gfx::present_mode(const gfx::present_modes &available_present_modes)> choose_present_mode = [](const gfx::present_modes &available_present_modes)
        {
            return gfx::present_mode::eImmediate;
        };

        context(uint32_t width, uint32_t height, const char *name);
        context(const context &) = delete;

        ~context();

        context &operator=(const context &) = delete;

        static context &current()
        {
            return *context::current_context;
        }

    private:
        const uint32_t width;
        const uint32_t height;

        std::string window_name;

        // vulkan objects
        vk::SwapchainKHR swap_chain;
        vk::Instance instance;
        vk::SurfaceKHR surface;

        std::vector<vk::Image> images;

        vk::Format swap_chain_image_format;
        vk::Extent2D swap_chain_extent;

        // devices
        vk::UniqueDevice device;
        std::optional<vk::PhysicalDevice> physical_device;

        static context *current_context;

        void init_window();
        void init_vulkan();
        void create_surface();
        void create_swap_chain();

        vk::Extent2D choose_swap_extent(const vk::SurfaceCapabilitiesKHR &capabilities);
    };
}