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
    {{ -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
    { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }},
    {  { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }},
    { { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f }}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

// initialize graphics context, device and swapchain
// this is just a simple testing environment/playground for me, this is not
// supposed to be used as a part of the library.
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

        gfx::vma_buffer<gfx::vertex> vertex_buffer(device, commands, vertices, vk::BufferUsageFlagBits::eVertexBuffer, vma::memory_usage::GpuOnly);
        gfx::vma_index_buffer<uint16_t> index_buffer(device, commands, indices, vma::memory_usage::GpuOnly, vk::IndexType::eUint16);

        // bind vertex buffer and attribute descriptions
        // clang-format off
        pipeline.bind_vertex_buffer<gfx::vertex>(
            gfx::vertex::get_binding_description(),
            gfx::vertex::get_attribute_descriptions()
        );
        // clang-format on

        // initialize the pipeline object
        pipeline.initialize();

        uint32_t frame_time = 0.0;
        double last_time = glfwGetTime();

        // create vertex buffer object
        // render loop
        while (!glfwWindowShouldClose(context->window))
        {

            frame_time++;
            auto current_time = glfwGetTime();
            auto elapsed_time = current_time - last_time;

            if (elapsed_time >= 1.0)
            {
                glfwSetWindowTitle(context->window, std::to_string(frame_time).c_str());
                frame_time = 0;
                last_time = current_time;
            }

            // begin drawing commands
            drawer.begin();

            // run commands within the draw object
            drawer.run([&](vk::CommandBuffer *buffer, auto index) {
                render_pass.begin(buffer, index, gfx::clear({ 0.0, 0.0, 0.0, 0.0 }));
                pipeline.bind<uint16_t>(buffer,
                    { vertex_buffer.get_buffer() },
                    { index_buffer });
                buffer->drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
                render_pass.end(buffer);
            });

            // poll for events
            glfwPollEvents();
        }

        // wait for device to finish rendering
        device->get_logical_device().waitIdle();
    } catch (std::exception &e)
    {
        spdlog::error("unable to instantiate vuxol, {}", e.what());
    }

    spdlog::info("program shutdown.");
}