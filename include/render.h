#pragma once

#include <commands.h>
#include <context.h>
#include <device.h>
#include <functional>
#include <global.h>
#include <swapchain/pipeline.h>
#include <swapchain/swapchain.h>

namespace gfx
{
    class draw
    {
    public:
        draw(std::shared_ptr<gfx::context> context);

        void begin();
        void run(
            std::function<void(vk::CommandBuffer *buffer, uint32_t image_index)> draw);

    private:
        std::shared_ptr<gfx::device> device;
        std::shared_ptr<gfx::context> context;
        std::shared_ptr<gfx::commands> commands;
        std::shared_ptr<gfx::swapchain> swapchain;
    };
}