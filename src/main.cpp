#include "device.h"
#include "spdlog/spdlog.h"
#include <context.h>
#include <render.h>
#include <swapchain.h>

int main()
{
    try
    {
        gfx::context context {};
        gfx::device device { &context.instance, &context.surface };
        gfx::swapchain swapchain { &device };

        context.init_swap_chain(swapchain);
    }
    catch (std::exception &e)
    {
        spdlog::error("unable to instantiate vuxol, {}", e.what());
    }
}