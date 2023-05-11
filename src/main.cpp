#include <GLFW/glfw3.h>
#include <buffer.h>
#include <context.h>
#include <device.h>
#include <memory>
#include <render.h>
#include <spdlog/spdlog.h>
#include <swapchain.h>
#include <util.h>
#include <vertex.h>

int main()
{
    try
    {
        auto context = std::make_shared<gfx::context>();

        auto device = std::make_shared<gfx::device>(&context->instance, &context->surface);
        auto swapchain = std::make_shared<gfx::swapchain>(device);

        // we have to initialize the swapchain __before__ we do anything else with the swapchain!
        context->init_swap_chain(swapchain);
        swapchain->add_render_pass(
            "shadow", gfx::start_render_pass(swapchain) // add a new render_pass to the swapchain by name "shadow"
        );

        auto render_pass = swapchain->render_passes.at("shadow");
        auto commands = std::make_shared<gfx::commands>(swapchain, &context->surface);

        context->device = device;
        context->commands = commands;

        gfx::draw drawer(context);
        gfx::pipeline pipeline {
            swapchain,
            "shadow",
            "build/triangle.vert.spv",
            "build/triangle.frag.spv",
        };

        // todo: think of a more elegant way to do this.
        // clang-format off
        pipeline.bind_struct<gfx::vertex>(
            gfx::vertex::get_binding_description(),
            gfx::vertex::get_attribute_descriptions()
        );
        // clang-format on

        pipeline.initialize();

        const std::vector<gfx::vertex> vertices = {
            {{ 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
            { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }},
            {{ -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }}
        };

        gfx::vertex_buffer<gfx::vertex, 3> vertex_buffer(device, vertices);

        while (!glfwWindowShouldClose(context->window))
        {
            drawer.begin();
            drawer.run([&](vk::CommandBuffer *buffer, auto index) {
                render_pass.begin(buffer, index, gfx::clear({ 0.0, 0.0, 0.0, 1.0 }));
                buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.vk_pipeline);
                buffer->bindVertexBuffers(0, vertex_buffer.get_buffer(), { 0 });
                buffer->draw(static_cast<uint32_t>(vertices.size()), 1, 0, 0);
                render_pass.end(buffer); // we have to end the render pass!
            });
            glfwPollEvents();
            device->logical_device.waitIdle();
        }
    } catch (std::exception &e)
    {
        spdlog::error("unable to instantiate vuxol, {}", e.what());
    }

    spdlog::info("program shutdown.");
}