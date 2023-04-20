#include <context.h>

namespace gfx
{
    context::context(uint32_t width, uint32_t height, const char *name) : width{width}, height{height}, windowName{name}
    {
        gfx::current_context = std::make_shared<gfx::context>(this);
        this->init_window();
    }

    context::~context()
    {
        glfwDestroyWindow(this->window);
        glfwTerminate();
    }

    void context::init_window()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        this->window = glfwCreateWindow(width, height, this->windowName.c_str(), nullptr, nullptr);
    }
}