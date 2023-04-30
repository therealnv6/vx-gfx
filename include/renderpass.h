#pragma once
#include <context.h>

namespace gfx
{
    class render_pass
    {
    public:
        vk::RenderPass pass;
        std::shared_ptr<gfx::context> context;

        render_pass(std::shared_ptr<gfx::context> );
        ~render_pass();

        void create_render_pass(std::shared_ptr<gfx::context> context);
    };
}