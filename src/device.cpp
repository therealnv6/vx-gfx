#include <algorithm>
#include <validation.h>
#include <device.h>
#include <global.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_enums.hpp>

namespace gfx
{
    device::device(const vk::Instance *instance, const vk::SurfaceKHR *surface)
    {
        assert(instance != nullptr);
        assert(surface != nullptr);

        std::vector<vk::PhysicalDevice> devices = instance->enumeratePhysicalDevices();

        vk::PhysicalDevice physical_device;
        gfx::queue_family_indices indices;

        auto suitable_device = find_most_suitable(devices, surface);

        if (!suitable_device.has_value())
        {
            throw std::runtime_error("unable to find suitable device!");
        }

        std::tie(physical_device, indices) = suitable_device.value();
        this->physical_device = physical_device;

        vk::DeviceQueueCreateInfo queue_create_info({}, indices.graphics_family.value(), 1, &this->queue_priority);
        vk::PhysicalDeviceFeatures device_features;

        vk::DeviceCreateInfo device_create_info({},
            static_cast<uint32_t>(1), &queue_create_info,
            0, nullptr, // validation layers, these will be filled later!
            device_extensions.size(), device_extensions.data(),
            &device_features);

        validation::fill_create_info(&device_create_info);

        this->logical_device = physical_device.createDevice(device_create_info);
        this->graphics_queue = this->logical_device.getQueue(indices.graphics_family.value(), 0);
    }

    device::~device()
    {
        this->cleanup();
    }

    void device::cleanup()
    {
        logical_device.destroy();
    }

    std::optional<std::pair<vk::PhysicalDevice, gfx::queue_family_indices>> device::find_most_suitable(
        const std::vector<vk::PhysicalDevice> devices,
        const vk::SurfaceKHR *surface)
    {
        // Evaluate each device and store the best one
        vk::PhysicalDevice best_device;
        gfx::queue_family_indices best_indices;

        int highest_score = -1;

        for (const auto &device : devices)
        {
            if (!this->device_suitable(device))
            {
                continue;
            }

            // Evaluate device properties to determine its score
            gfx::queue_family_indices indices = this->find_queue_families(std::optional(device), surface, this->queue_flags);
            int score = evaluate_device(device, indices);

            // If the score is higher than the previous best, update the best device and score
            if (score > highest_score)
            {
                best_device = device;
                best_indices = indices;
                highest_score = score;
            }
        }

        // Return the best device, if any were found
        if (highest_score >= 0)
        {
            return std::pair(best_device, best_indices);
        }
        else
        {
            return std::nullopt;
        }
    }

    gfx::queue_family_indices device::find_queue_families(
        const std::optional<vk::PhysicalDevice> device,
        const vk::SurfaceKHR *surface,
        const vk::QueueFlags flag_bits)
    {
        std::vector<vk::QueueFamilyProperties> queue_family_properties = device->getQueueFamilyProperties();
        gfx::queue_family_indices indices;

        uint32_t i = 0;

        for (const auto &queue_family : queue_family_properties)
        {
            if (indices.is_complete())
            {
                break;
            }

            if (queue_family.queueFlags & flag_bits)
            {
                indices.graphics_family = i;

                if (device->getSurfaceSupportKHR(i, *surface))
                {
                    indices.present_family = i;
                }
            }
        }

        return indices;
    }

    uint32_t device::evaluate_device(vk::PhysicalDevice physical_device, gfx::queue_family_indices indices)
    {
        return indices.is_complete() ? 1000 : 0;
    }
}