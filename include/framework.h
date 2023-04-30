#pragma once
#include <context.h>
#include <pipeline.h>

namespace gfx
{
    class framework
    {
    public:
        framework(std::shared_ptr<gfx::context> context)
            : context{context}
        {
        }

        bool should_window_close();
        void run(gfx::pipeline pipeline);

    private:
        std::shared_ptr<gfx::context> context;
    };
}