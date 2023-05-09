#pragma once
#include <context.h>
#include <vulkan/vulkan.hpp>

namespace gfx
{
    vk::ClearValue clear(std::array<float, 4> f32);
}