#pragma once
#include <context.h>

namespace gfx
{
    class render_pass
    {
    public:
        vk::RenderPass pass;

        render_pass(gfx::context &context);

        void create_render_pass(gfx::context &context);
    };
}