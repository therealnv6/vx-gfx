#include "device.h"
#include <context.h>
#include <render.h>
#include <swapchain.h>

int main()
{
    gfx::device device { nullptr, nullptr };
    gfx::context context {};
    gfx::swapchain swapchain { &context };

    context.init_swap_chain(swapchain);
}