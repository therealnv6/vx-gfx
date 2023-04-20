#pragma once

#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include <GLFW/glfw3.h>

namespace gfx
{
    static std::shared_ptr<context> current_context = nullptr;

    class context
    {
    public:
        GLFWwindow *window;

        context(uint32_t width, uint32_t height, const char *name);
        ~context();

        context(const context &) = delete;
        context &operator=(const context &) = delete;

        context *operator->()
        {
            return this;
        }

        static std::shared_ptr<context> current()
        {
            return current_context;
        }

    private:
        const uint32_t width;
        const uint32_t height;

        std::string windowName;

        void init_window();
    };
}