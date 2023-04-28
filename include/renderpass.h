#pragma once
#include <context.h>

namespace gfx
{
    class render_pass
    {
    public:
        vk::RenderPass render_pass;

        void create_render_pass(gfx::context &context);
    };
}