#include "device.h"
#include <commands.h>
#include <config.h>

namespace gfx
{
    commands::commands(gfx::swapchain *swapchain, vk::SurfaceKHR *surface)
        : swapchain { swapchain }
        , device { swapchain->device }
        , surface { surface }
    {
        this->create_sync_objects();
        this->create_command_pool();
        this->create_command_buffer();
    }

    commands::~commands()
    {
        this->cleanup();
    }

    void commands::cleanup()
    {
        spdlog::warn("todo: clean up commands!");
    }

    void commands::create_command_pool()
    {
        gfx::queue_family_indices indices = device->find_queue_families(this->surface, device->physical_device);
        vk::CommandPoolCreateInfo pool_info {
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer, indices.graphics_family.value()
        };

        this->command_pool = device->logical_device.createCommandPool(pool_info);
    }

    void commands::create_command_buffer()
    {
        this->command_buffers = device->logical_device.allocateCommandBuffers(
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
            this->image_available_semaphores.push_back(device->logical_device.createSemaphore(semaphore_info));
            this->render_finished_semaphores.push_back(device->logical_device.createSemaphore(semaphore_info));
            this->in_flight_fences.push_back(device->logical_device.createFence(fence_info));
        }
    }
}