#include <framework.h>
#include <vulkan/vulkan_core.h>

namespace gfx
{
    bool framework::should_window_close()
    {
        return glfwWindowShouldClose(this->context->window);
    }

    void framework::run(gfx::pipeline pipeline)
    {
        while (!this->should_window_close())
        {
            glfwPollEvents();

            context->draw([this, &pipeline](vk::CommandBuffer *buffer, uint32_t image_index)
                {
                    vk::ClearValue clear_value({ 0.0f, 0.0f, 0.0f, 1.0f });
                    vk::Rect2D scissor {
                        { 0, 0 },
                        context->swap_chain_extent
                    };

                    vk::Viewport viewport {
                        0.0f, 0.0f, static_cast<float>(context->swap_chain_extent.width), static_cast<float>(context->swap_chain_extent.height), 0.0f, 1.0f
                    };

                    vk::RenderPassBeginInfo render_pass_info {
                        pipeline.render_pass.pass,
                        pipeline.render_pass.swap_chain_framebuffers[image_index],
                        scissor,
                        1,
                        &clear_value,
                    };

                    buffer->beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
                    buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.graphics_pipeline);
                    buffer->setViewport(0, viewport);
                    buffer->setScissor(0, scissor);
                    buffer->draw(6, 1, 0, 0);

                    buffer->endRenderPass();

                    if (vkEndCommandBuffer(*buffer) != VK_SUCCESS)
                    {
                        throw std::runtime_error("failed to end command buffer!");
                    } //
                });
        }
    }
}