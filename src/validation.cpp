#include <spdlog/spdlog.h>
#include <stdexcept>
#include <validation.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace validation
{
    typedef vk::DebugUtilsMessageSeverityFlagBitsEXT severity_flags;
    typedef vk::DebugUtilsMessageTypeFlagBitsEXT message_type_flag;

    inline VkResult create_debug_utils_messenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    inline void destroy_debug_utils_messenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData)
    {
        auto severity = static_cast<severity_flags>(messageSeverity);

        if (severity == severity_flags::eError)
        {
            spdlog::error("validation layer: {}", pCallbackData->pMessage);
        }
        else if (severity == severity_flags::eInfo)
        {
            spdlog::info("validation layer: {}", pCallbackData->pMessage);
        }
        else if (severity == severity_flags::eWarning)
        {
            spdlog::warn("validation layer: {}", pCallbackData->pMessage);
        }

        return VK_FALSE;
    }

    void fill_create_info(vk::DeviceCreateInfo *info)
    {
        if (enable_validation_layers() && !check_validation_layer_support())
        {
            throw std::runtime_error("validation layers requested, but not available");
        }

        // [warning] validation layer: loader_create_device_chain: Using deprecated and ignored 'ppEnabledLayerNames' member of 'VkDeviceCreateInfo' when creating a Vulkan device.
        info->enabledLayerCount = static_cast<uint32_t>(ENABLED_LAYERS.size());
        info->ppEnabledLayerNames = ENABLED_LAYERS.data();
    }

    vk::DebugUtilsMessengerEXT create_debug_messenger(vk::Instance *instance)
    {
        if (!enable_validation_layers())
        {
            return nullptr;
        }

        auto severity = severity_flags::eInfo
            | severity_flags::eWarning
            | severity_flags::eError
            | severity_flags::eWarning;

        auto type = message_type_flag::eGeneral
            | message_type_flag::eValidation
            | message_type_flag::ePerformance;

        vk::DebugUtilsMessengerCreateInfoEXT createInfo({},
            severity, // messageSeverity
            type, // messageType
            validation::debug_callback // pfnUserCallback
        );

        auto legacy_create_info = static_cast<VkDebugUtilsMessengerCreateInfoEXT>(createInfo);
        VkDebugUtilsMessengerEXT debug_target;

        VkResult result = create_debug_utils_messenger(*instance, &legacy_create_info, nullptr, &debug_target);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up debug messenger: " + std::to_string(result));
        }

        return debug_target;
    }

    const bool enable_validation_layers()
    {
        return !ENABLED_LAYERS.empty();
    }

    const bool check_validation_layer_support()
    {
        std::vector<vk::LayerProperties> available_layers = vk::enumerateInstanceLayerProperties();

        for (const char *layer_name : ENABLED_LAYERS)
        {
            bool layer_found = false;

            for (const auto &layer_properties : available_layers)
            {
                if (strcmp(layer_name, layer_properties.layerName) == 0)
                {
                    layer_found = true;
                    break;
                }
            }

            if (!layer_found)
            {
                return false;
            }
        }

        return true;
    }
}