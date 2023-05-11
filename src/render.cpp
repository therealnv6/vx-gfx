#include "commands.h"
#include "device.h"
#include <config.h>
#include <render.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace gfx
{
    draw::draw(std::shared_ptr<gfx::context> context)
        : context { context }
        , device { context->device }
        , commands { context->commands }
        , swapchain {
            context->swapchain
        }
    {
        assert(context != nullptr);
        assert(device != nullptr);
        assert(commands != nullptr);
        assert(swapchain != nullptr);
    }

    void draw::begin()
    {
        commands->begin(commands->command_buffers[commands->current_frame]);
    }

    void draw::run(
        std::function<void(vk::CommandBuffer *buffer, uint32_t image_index)> draw)
    {
        vk::ResultValue<uint32_t> image_index = device->logical_device.acquireNextImageKHR(
            swapchain->chain,
            UINT64_MAX,
            commands->image_available_semaphores[commands->current_frame]);

        draw(&commands->command_buffers[commands->current_frame], image_index.value);
        commands->command_buffers[commands->current_frame].end();

        spdlog::debug("just passed draw()");

        // we can't use commands->wait_and_submit() here, because we also have to signal the semaphores!
        // however, we don't always want to signal them, that's why the wait_and_submit() function doesn't do this.
        vk::Semaphore wait_semaphores[] = { commands->image_available_semaphores[commands->current_frame] };
        vk::Semaphore signal_semaphores[] = { commands->render_finished_semaphores[commands->current_frame] };

        vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

        vk::SubmitInfo submit_info {
            sizeof(wait_semaphores) / sizeof(vk::Semaphore),
            wait_semaphores,
            wait_stages,
            1,
            &commands->command_buffers[commands->current_frame],
            sizeof(signal_semaphores) / sizeof(vk::Semaphore),
            signal_semaphores,
        };

        spdlog::debug("attempting to submit to device->graphics_queue");
        device->graphics_queue.submit(submit_info, commands->in_flight_fences[commands->current_frame]);
        spdlog::debug("submmited to device->graphics_queue");

        vk::SwapchainKHR swap_chains[] = { swapchain->chain };
        vk::PresentInfoKHR present_info {
            sizeof(wait_semaphores) / sizeof(vk::Semaphore),
            signal_semaphores,
            sizeof(swap_chains) / sizeof(vk::SwapchainKHR),
            swap_chains,
            &image_index.value
        };

        spdlog::debug("trying to present to the surface");
        if (device->present_queue.presentKHR(present_info) != vk::Result::eSuccess)
        {
            throw std::runtime_error("unable to present info!");
        }

        commands->current_frame = (commands->current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
}