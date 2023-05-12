#include "device.h"
#include <commands.h>
#include <config.h>
#include <vulkan/vulkan_handles.hpp>

namespace gfx
{
    commands::commands(std::shared_ptr<gfx::swapchain> swapchain, vk::SurfaceKHR *surface)
        : swapchain { swapchain }
        , device { swapchain->device }
        , surface { surface }
    {
        this->create_sync_objects();
        this->create_command_pool();
        this->initialize_command_buffers();
    }

    commands::~commands()
    {
        this->cleanup();
    }

    void commands::cleanup()
    {
        spdlog::info("cleaning up gfx::commands");
        device->get_logical_device().destroyCommandPool(this->command_pool);
        spdlog::info("... done!");
    }

    void commands::create_command_pool()
    {
        gfx::queue_family_indices indices = device->find_queue_families(this->surface, device->get_physical_device());
        vk::CommandPoolCreateInfo pool_info {
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer, indices.graphics_family.value()
        };

        this->command_pool = device->get_logical_device().createCommandPool(pool_info);
    }

    void commands::initialize_command_buffers()
    {
        this->command_buffers = device->get_logical_device().allocateCommandBuffers(
            vk::CommandBufferAllocateInfo(
                this->command_pool,
                vk::CommandBufferLevel::ePrimary,
                MAX_FRAMES_IN_FLIGHT));
    }

    void commands::create_sync_objects()
    {
        vk::SemaphoreCreateInfo semaphore_info;
        vk::FenceCreateInfo fence_info { vk::FenceCreateFlagBits::eSignaled };

        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            this->image_available_semaphores.push_back(device->get_logical_device().createSemaphore(semaphore_info));
            this->render_finished_semaphores.push_back(device->get_logical_device().createSemaphore(semaphore_info));
            this->in_flight_fences.push_back(device->get_logical_device().createFence(fence_info));
        }
    }

    vk::CommandBuffer commands::start_small_buffer()
    {
        return device->get_logical_device().allocateCommandBuffers(
            vk::CommandBufferAllocateInfo(
                this->command_pool,
                vk::CommandBufferLevel::ePrimary,
                1))[0];
    }

    void commands::begin(const vk::CommandBuffer &command_buffer)
    {
        if (device->get_logical_device().waitForFences(in_flight_fences[current_frame], true, UINT64_MAX) != vk::Result::eSuccess)
        {
            throw std::runtime_error("unable to wait for fence in_flight_fence!");
        }

        device->get_logical_device().resetFences(in_flight_fences[current_frame]);

        command_buffer.reset(); // reset command buffer
        command_buffer.begin(vk::CommandBufferBeginInfo {});
    }

    void commands::submit_and_wait(const vk::CommandBuffer &command_buffer)
    {
        command_buffer.end();

        vk::Fence *fence = &in_flight_fences[current_frame];
        vk::SubmitInfo submit_info(0, nullptr, nullptr, 1, &command_buffer, 0, nullptr);

        device->graphics_queue.submit(submit_info, *fence);

        vk::Result result = device->get_logical_device().waitForFences(*fence, VK_TRUE, UINT64_MAX);

        if (result != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to wait for fence");
        }
    }
}