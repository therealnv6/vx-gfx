#define VMA_IMPLEMENTATION
#define VMA_VULKAN_VERSION 1003000
#define VMA_DEBUG_REPORT 1
#include "vk_mem_alloc.h"
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
#include <vulkan/vulkan_enums.hpp>

const std::vector<gfx::vertex> vertices = {
    {{ 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
    { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }},
    {{ -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }}
};

// initialize graphics context, device and swapchain
int main()
{
    try
    {
        // create shared context object
        auto context = std::make_shared<gfx::context>();

        // create shared device object
        auto device = std::make_shared<gfx::device>(&context->instance, &context->surface);

        // create shared swapchain object
        auto swapchain = std::make_shared<gfx::swapchain>(device);

        // initialize swapchain before doing anything else with it
        context->init_swap_chain(swapchain);

        // add new render pass to swapchain by name "shadow"
        swapchain->add_render_pass(
            "shadow", gfx::start_render_pass(swapchain));

        // get the render pass from the swapchain
        auto render_pass = swapchain->render_passes.at("shadow");

        // create shared commands object
        auto commands = std::make_shared<gfx::commands>(swapchain, &context->surface);

        // set device and commands for the context
        context->device = device;
        context->commands = commands;

        // create draw object
        gfx::draw drawer(context);

        // create pipeline object with specified parameters
        gfx::pipeline pipeline {
            swapchain,
            "shadow",
            "build/triangle.vert.spv",
            "build/triangle.frag.spv",
        };

        // bind vertex buffer and attribute descriptions
        // clang-format off
        pipeline.bind_struct<gfx::vertex>(
            gfx::vertex::get_binding_description(),
            gfx::vertex::get_attribute_descriptions()
        );
        // clang-format on

        // initialize the pipeline object
        pipeline.initialize();

        // create vertex buffer object
        gfx::vma_buffer<gfx::vertex> vertex_buffer(device, commands, vertices, vk::BufferUsageFlagBits::eVertexBuffer, VMA_MEMORY_USAGE_GPU_ONLY);

        // render loop
        while (!glfwWindowShouldClose(context->window))
        {
            // begin drawing commands
            drawer.begin();

            // run commands within the draw object
            drawer.run([&](vk::CommandBuffer *buffer, auto index) {
                // begin render pass
                render_pass.begin(buffer, index, gfx::clear({ 0.0, 0.0, 0.0, 0.0 }));

                // bind pipeline
                buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.vk_pipeline);

                // bind vertex buffer
                buffer->bindVertexBuffers(0, vertex_buffer.get_buffer(), { 0 });

                // draw vertices
                buffer->draw(static_cast<uint32_t>(vertices.size()), 1, 0, 0);

                // end render pass
                render_pass.end(buffer);
            });

            // poll for events
            glfwPollEvents();

            // wait for device to finish rendering
            device->logical_device.waitIdle();
        }
    } catch (std::exception &e)
    {
        spdlog::error("unable to instantiate vuxol, {}", e.what());
    }

    spdlog::info("program shutdown.");
}