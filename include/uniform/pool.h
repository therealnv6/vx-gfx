#pragma once
#include <config.h>
#include <device.h>
#include <stdexcept>
#include <uniform/layout.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace gfx
{
    class descriptor_pool
    {
    public:
        descriptor_pool(std::shared_ptr<gfx::device> device, vk::DescriptorType type)
            : type { type }
        {
            this->create_pool();
        };

        ~descriptor_pool()
        {
            device->get_logical_device().destroyDescriptorPool(this->pool);
        }

        std::vector<vk::DescriptorSet> create_descriptor_sets(gfx::uniform_layout layout)
        {
            std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout.layout);
            std::vector<vk::DescriptorSet> sets(MAX_FRAMES_IN_FLIGHT);

            vk::DescriptorSetAllocateInfo allocate {
                this->pool,
                static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
                layouts.data()
            };

            auto result = device->get_logical_device().allocateDescriptorSets(&allocate, sets.data());

            if (result != vk::Result::eSuccess)
            {
                throw std::runtime_error("error while allocating descriptor sets!");
            }

            return sets;
        }
        
        std::shared_ptr<gfx::device> device;

    private:
        vk::DescriptorType type;
        vk::DescriptorPool pool;

        void create_pool()
        {
            vk::DescriptorPoolSize size { type, MAX_FRAMES_IN_FLIGHT };
            vk::DescriptorPoolCreateInfo info { {}, MAX_FRAMES_IN_FLIGHT, 1, &size };

            this->pool = device->get_logical_device().createDescriptorPool(info);
        }
    };
}