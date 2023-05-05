#pragma once
#include <functional>
#include <global.h>
#include <optional>

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

        vk::Queue graphics_queue;
        vk::Queue present_queue;

        // Function for checking if a physical device is suitable for use.
        // This function takes a physical device as input and returns a boolean value.
        std::function<bool(vk::PhysicalDevice)> device_suitable = [](vk::PhysicalDevice device)
        {
            // Default is that all devices are suitable for use.
            return true;
        };

        device(vk::Instance *instance, vk::SurfaceKHR *surface);
        ~device();

        device(const device &) = delete;
        device operator=(const device &) = delete;

    private:
        float queue_priority = 1.0f;

        std::optional<std::pair<vk::PhysicalDevice, gfx::queue_family_indices>> find_most_suitable(
            std::vector<vk::PhysicalDevice> device,
            vk::SurfaceKHR *surface);

        gfx::queue_family_indices find_queue_families(
            std::optional<vk::PhysicalDevice> device,
            vk::SurfaceKHR *surface,
            const vk::QueueFlagBits flag_bits);

        uint32_t evaluate_device(vk::PhysicalDevice physical_device, gfx::queue_family_indices indices);

        void cleanup();
    };
}