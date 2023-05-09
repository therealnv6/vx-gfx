#pragma once
#include "spdlog/spdlog.h"
#include <iostream>
#include <optional>
#include <vulkan/vulkan.hpp>

namespace gfx
{
    struct queue_family_indices {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        bool is_complete()
        {
            return graphics_family.has_value() && present_family.has_value();
        }
    };

    typedef vk::PresentModeKHR present_mode;
    typedef vk::SurfaceFormatKHR surface_format;

    typedef std::vector<present_mode> present_modes;
    typedef std::vector<surface_format> surface_formats;
}