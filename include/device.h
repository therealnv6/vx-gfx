#pragma once
#include <global.h>
#include <optional>
#include <vulkan/vulkan_handles.hpp>

namespace gfx
{
    static const std::vector<const char *> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME,
        VK_KHR_MAINTENANCE2_EXTENSION_NAME,
        VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME,
    };

    static const std::vector<const char *> validation_layers = {
        "VK_LAYER_KHRONOS_validation",
    };

    class device
    {
    public:
        vk::Device logical_device;
        vk::PhysicalDevice physical_device;

        device(vk::Instance *instance, vk::SurfaceKHR *surface);

        device(const device &) = delete;
        device operator=(const device &) = delete;

    private:
        float queue_priority = 1.0f;

        std::optional<std::pair<vk::PhysicalDevice, gfx::queue_family_indices>> find_most_suitable(std::vector<vk::PhysicalDevice> device, vk::SurfaceKHR *surface);
        gfx::queue_family_indices find_queue_families(std::optional<vk::PhysicalDevice> device, vk::SurfaceKHR *surface, const vk::QueueFlagBits flag_bits);
        uint32_t evaluate_device(vk::PhysicalDevice physical_device, gfx::queue_family_indices indices);
    };
}