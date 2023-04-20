#include <framework.h>

namespace gfx
{
    bool framework::should_window_close()
    {
        return glfwWindowShouldClose(this->window);
    }

    void framework::run()
    {
        while (!this->should_window_close())
        {
            glfwPollEvents();
        }
    }
}