#pragma once
#include <context.h>

namespace gfx
{
    class framework
    {
    public:
        framework(GLFWwindow *window)
            : window { window }
        {
        }

        bool should_window_close();
        void run();

    private:
        GLFWwindow *window;
    };
}