#include <stdexcept>
#include <validation.h>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace validation
{
    void fill_create_info(vk::DeviceCreateInfo *info)
    {
        if (enable_validation_layers() && !check_validation_layer_support())
        {
            throw std::runtime_error("validation layers requested, but not available");
        }

        info->enabledLayerCount = static_cast<uint32_t>(ENABLED_LAYERS.size());
        info->ppEnabledLayerNames = ENABLED_LAYERS.data();
    }

    bool enable_validation_layers()
    {
        return !ENABLED_LAYERS.empty();
    }

    bool check_validation_layer_support()
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