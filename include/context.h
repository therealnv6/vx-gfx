#pragma once

#define GLFW_INCLUDE_VULKAN

#include <cstdlib>
#include <iostream>
#include <stdexcept>


#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp> // modern c++!


namespace gfx
{
    class context
    {
    public:
        GLFWwindow *window;

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
        VkInstance instance;
        static context *current_context;

        void init_window();
        void init_vulkan();
    };
}