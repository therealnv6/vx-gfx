#pragma once
#include <global.h>
#include <vulkan/vulkan_structs.hpp>

namespace validation
{
    const std::vector<const char *> ENABLED_LAYERS = {
// #ifdef NDEBUG // only have validation layers if NDEBUG is enabled.
        "VK_LAYER_KHRONOS_validation"
// #endif
    };

    void fill_create_info(vk::DeviceCreateInfo *info);
    vk::DebugUtilsMessengerEXT create_debug_messenger(vk::Instance *instance);

    const bool enable_validation_layers();
    const bool check_validation_layer_support();
}