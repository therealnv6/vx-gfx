#pragma once

#define GLFW_INCLUDE_VULKAN

#include <cstdlib>
#include <functional>
#include <iostream>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp> // modern c++!

#include <optional>

namespace gfx
{
    struct queue_family_indices
    {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        bool is_complete()
        {
            return graphics_family.has_value() && present_family.has_value();
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
        { return true; };

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
        vk::Instance instance;
        vk::SurfaceKHR surface;

        static context *current_context;

        void init_window();
        void init_vulkan();
        void create_surface();
    };
}