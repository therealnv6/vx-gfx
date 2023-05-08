#include <algorithm>
#include <device.h>
#include <global.h>
#include <optional>
#include <set>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <validation.h>
#include <vector>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

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

        this->logical_device = physical_device.createDevice(device_create_info);
        this->graphics_queue = logical_device.getQueue(indices.graphics_family.value(), 0);
        this->present_queue = logical_device.getQueue(indices.present_family.value(), 0);
    }

    device::~device()
    {
        this->cleanup();
    }

    void device::cleanup()
    {
        spdlog::info("cleaning up gfx::device");
        logical_device.destroy();
        spdlog::info("... done!");
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
            gfx::queue_family_indices indices = this->find_queue_families(surface, device);
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

    gfx::queue_family_indices device::find_queue_families(const vk::SurfaceKHR *surface, std::optional<vk::PhysicalDevice> device)
    {
        if (device == std::nullopt)
        {
            throw std::runtime_error("device provided in find_queue_families() was std::nullopt");
        }

        std::vector<vk::QueueFamilyProperties> queue_family_properties = device->getQueueFamilyProperties();
        gfx::queue_family_indices indices;

        uint32_t i = 0;

        for (const auto &queue_family : queue_family_properties)
        {
            if (indices.is_complete())
            {
                break;
            }

            if (queue_family.queueFlags & queue_flags)
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
        int evaluation = 0;
        bool extensions_supported = false;

        std::vector<vk::ExtensionProperties> available_extensions = physical_device.enumerateDeviceExtensionProperties();
        std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

        for (const auto &extension : available_extensions)
        {
            required_extensions.erase(extension.extensionName);
        }

        if (!required_extensions.empty())
        {
            return 0;
        }

        if (!indices.is_complete())
        {
            return 0;
        }

        return evaluation++;
    }
}