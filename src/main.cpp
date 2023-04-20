#include <context.h>
#include <framework.h>
#include <pipeline.h>

int main()
{
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    try
    {
        gfx::context context(800, 600, "vx-gfx");
        gfx::framework framework(context.window);
        gfx::pipeline pipeline{"shaders/triangle.vert.spv", "shaders/triangle.frag.spv"};

        framework.run();
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}