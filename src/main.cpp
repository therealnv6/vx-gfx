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

        // we have to initialize the swapchain __before__ we do anything else with the swapchain!
        context.init_swap_chain(&swapchain);

        swapchain.add_render_pass(
            "shadow", gfx::start_render_pass(&swapchain) // add a new render_pass to the swapchain by name "shadow"
        );

        gfx::render_pass *render_pass = &swapchain.render_passes.at("shadow");
        gfx::commands commands { &swapchain, &context.surface };

        context.device = &device;
        context.commands = &commands;

        gfx::draw drawer(&context);
        gfx::pipeline pipeline { &swapchain,
            "shadow",
            "build/triangle.vert.spv",
            "build/triangle.frag.spv" };

        drawer.begin();
        drawer.run([&](auto buffer, auto index)
            {
                vk::ClearValue clear_value({ 0.0f, 0.0f, 0.0f, 1.0f });
                vk::Rect2D scissor {
                    { 0, 0 },
                    swapchain.extent
                };

                vk::Viewport viewport {
                    0.0f, 0.0f, static_cast<float>(swapchain.extent.width), static_cast<float>(swapchain.extent.height), 0.0f, 1.0f
                };

                vk::RenderPassBeginInfo render_pass_info {
                    render_pass->pass,
                    render_pass->framebuffers[index],
                    scissor,
                    1,
                    &clear_value,
                };

                buffer->beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
                buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.vk_pipeline);
                buffer->setViewport(0, viewport);
                buffer->setScissor(0, scissor);
                buffer->draw(6, 1, 0, 0);
                buffer->endRenderPass(); //
            });
    }
    catch (std::exception &e)
    {
        spdlog::error("unable to instantiate vuxol, {}", e.what());
    }

    spdlog::info("program shutdown.");
}