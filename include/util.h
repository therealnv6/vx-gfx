#pragma once
#include <context.h>
#include <vulkan/vulkan.hpp>

namespace gfx
{
    vk::ClearValue clear(std::array<float, 4> f32);

    void begin_render_pass(
        gfx::render_pass *render_pass,
        gfx::swapchain *swapchain,
        uint32_t index,
        vk::CommandBuffer *buffer,
        vk::Rect2D scissor,
        vk::ClearValue clear);
}