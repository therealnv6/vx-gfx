#include <util.h>

namespace gfx
{
    vk::ClearValue clear(std::array<float, 4> f32)
    {
        return vk::ClearValue {
            f32
        };
    }

    void begin_render_pass(
        gfx::render_pass *render_pass,
        gfx::swapchain *swapchain,
        uint32_t index,
        vk::CommandBuffer *buffer,
        vk::Rect2D scissor,
        vk::ClearValue clear)
    {
        vk::Viewport viewport {
            0.0f, 0.0f, static_cast<float>(swapchain->extent.width), static_cast<float>(swapchain->extent.height), 0.0f, 1.0f
        };

        vk::RenderPassBeginInfo render_pass_info {
            render_pass->pass,
            render_pass->framebuffers[index],
            scissor,
            1,
            &clear,
        };

        buffer->beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
        buffer->setViewport(0, viewport);
        buffer->setScissor(0, scissor);
    }
}