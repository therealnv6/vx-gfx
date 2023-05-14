#pragma once
#include <device.h>
#include <memory>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace gfx
{
    class uniform_layout
    {
    public:
        vk::DescriptorSetLayout layout;

        uniform_layout(std::shared_ptr<gfx::device> device, vk::DescriptorSetLayoutBinding binding, vk::DescriptorSetLayoutCreateInfo create_info)
        {
            create_info.setBindings(binding);
            
            auto result = device->get_logical_device().createDescriptorSetLayout(&create_info, nullptr, &layout);

            if (result != vk::Result::eSuccess)
            {
                throw std::runtime_error("unable to create descriptor layout with binding!");
            }
        }
    };
}