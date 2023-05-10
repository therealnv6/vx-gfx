#include "GLFW/glfw3.h"
#include "device.h"
#include "spdlog/spdlog.h"
#include <buffer.h>
#include <context.h>
#include <render.h>
#include <swapchain.h>
#include <util.h>
#include <vertex.h>

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

        gfx::render_pass render_pass = swapchain.render_passes.at("shadow");
        gfx::commands commands { &swapchain, &context.surface };

        context.device = &device;
        context.commands = &commands;

        gfx::draw drawer(&context);
        gfx::pipeline pipeline {
            &swapchain,
            "shadow",
            "build/triangle.vert.spv",
            "build/triangle.frag.spv",
        };

        // todo: think of a more elegant way to do this. 
        pipeline.bind_struct<gfx::vertex>(
            gfx::vertex::get_binding_description(),
            gfx::vertex::get_attribute_descriptions()
        );

        pipeline.initialize();

        const std::vector<gfx::vertex> vertices = {
            {{ 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
            { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }},
            {{ -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }}
        };

        gfx::vertex_buffer<gfx::vertex, 3> vertex_buffer(&device, vertices);


        while (!glfwWindowShouldClose(context.window))
        {
            drawer.begin();
            drawer.run([&](vk::CommandBuffer *buffer, auto index) {
                render_pass.begin(buffer, index, gfx::clear({ 0.0, 0.0, 0.0, 1.0 }));
                buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.vk_pipeline);
                buffer->bindVertexBuffers(0, vertex_buffer.inner(), { 0 });
                buffer->draw(static_cast<uint32_t>(vertices.size()), 1, 0, 0);
                render_pass.end(buffer); // we have to end the render pass!
            });
            glfwPollEvents();
            device.logical_device.waitIdle();
        }
    } catch (std::exception &e)
    {
        spdlog::error("unable to instantiate vuxol, {}", e.what());
    }

    spdlog::info("program shutdown.");
}