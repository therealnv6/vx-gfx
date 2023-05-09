#pragma once

#include <commands.h>
#include <context.h>
#include <device.h>
#include <functional>
#include <global.h>
#include <pipeline.h>
#include <swapchain.h>

namespace gfx
{
    class draw
    {
    public:
        draw(gfx::context *context);

        void begin();
        void run(
            std::function<void(vk::CommandBuffer *buffer, uint32_t image_index)> draw);

    private:
        gfx::device *device;
        gfx::context *context;
        gfx::commands *commands;
        gfx::swapchain *swapchain;
    };
}