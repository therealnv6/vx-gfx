#include "config.h"
#include <buffer/buffer.h>
#include <device.h>
#include <stdexcept>
#include <type_traits>
#include <util.h>
#include <vertex.h>
#include <vulkan/vulkan_core.h>

namespace gfx
{

    template<class T>
    buffer<T>::buffer(std::shared_ptr<gfx::device> device, std::shared_ptr<gfx::commands> commands, const T &data, size_t size, vk::BufferUsageFlags usage, vma::memory_usage memory_usage)
        : device(device)
        , size(size)
    {
        VkBuffer staging_buffer;
        VmaAllocation staging_allocation;

        vk::BufferCreateInfo staging_buffer_info({}, size, vk::BufferUsageFlagBits::eTransferSrc);
        VmaAllocationCreateInfo staging_alloc_info = { 0, VMA_MEMORY_USAGE_CPU_ONLY };

        VkBufferCreateInfo staging_info = static_cast<VkBufferCreateInfo>(staging_buffer_info);

        if (usage & vk::BufferUsageFlagBits::eTransferDst)
        {
            // Create the staging buffer.
            vmaCreateBuffer(device->get_vma_allocator(),
                &staging_info,
                &staging_alloc_info,
                &staging_buffer,
                &staging_allocation,
                nullptr);

            void *staging_data;
            vmaMapMemory(device->get_vma_allocator(), staging_allocation, &staging_data);

            if constexpr (std::is_const_v<T> && std::is_pointer_v<T>)
            {
                memcpy(staging_data, static_cast<const void *>(data), size);
            }
            else
            {
                memcpy(staging_data, reinterpret_cast<const void *>(&data), size);
            }

            vmaUnmapMemory(device->get_vma_allocator(), staging_allocation);
        }

        vk::BufferCreateInfo device_buffer_info({}, size, usage, vk::SharingMode::eExclusive);
        VkBufferCreateInfo device_info = static_cast<VkBufferCreateInfo>(device_buffer_info);

        // Create the device buffer(s).

        if (usage & vk::BufferUsageFlagBits::eTransferDst)
        {
            VmaAllocationCreateInfo device_alloc_info = { 0, vma::to_vma_memory_usage(memory_usage) };

            // Create a single device buffer.
            VkBuffer buffer;
            VmaAllocation allocation;

            vmaCreateBuffer(device->get_vma_allocator(),
                &device_info,
                &device_alloc_info,
                &buffer,
                &allocation,
                nullptr);

            this->allocations.push_back(allocation);
            this->buffers.push_back(buffer);
        }
        else
        {

            // Create multiple device buffers, one for each frame in flight.
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                VkBuffer buffer;
                VmaAllocation allocation;
                VmaAllocationCreateInfo device_alloc_info = { 0, vma::to_vma_memory_usage(memory_usage), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT };

                void *mapped_data = malloc(size);

                vmaCreateBuffer(device->get_vma_allocator(),
                    &device_info,
                    &device_alloc_info,
                    &buffer,
                    &allocation,
                    nullptr);

                memcpy(&mapped_data, reinterpret_cast<const void *>(&data), size);
                vmaMapMemory(device->get_vma_allocator(), allocation, &mapped_data);

                this->allocations.push_back(allocation);
                this->buffers.emplace_back(buffer);
                this->data_mapped.push_back(mapped_data);
                spdlog::info("buffers: MFIF={}, buffers.size()={}, index={}, T={}", MAX_FRAMES_IN_FLIGHT, buffers.size(), i, typeid(T).name());
            }
        }

        if (usage & vk::BufferUsageFlagBits::eTransferDst)
        {
            // Copy data from the staging buffer to the device buffer.
            vk::CommandBuffer buffer = commands->start_small_buffer();
            commands->begin(buffer);

            vk::BufferCopy copy_region(0, 0, size);

            buffer.copyBuffer(staging_buffer, this->buffers[0], 1, &copy_region);
            commands->submit_and_wait(buffer);

            vmaDestroyBuffer(device->get_vma_allocator(), staging_buffer, staging_allocation);
        }
    }

    template<class T>
    buffer<T>::~buffer()
    {
        for (int i = 0; i < this->buffers.size(); i++)
        {
            vmaDestroyBuffer(device->get_vma_allocator(), buffers[i], allocations[i]);
        }
    }
}