#include <X11/Xresource.h>
#include <buffer/buffer.h>
#include <device.h>
#include <stdexcept>
#include <util.h>
#include <vertex.h>
#include <vulkan/vulkan_core.h>

namespace gfx
{
    template<typename T>
    buffer<T>::buffer(std::shared_ptr<gfx::device> device, std::shared_ptr<gfx::commands> commands, const T &data, size_t size, vk::BufferUsageFlags usage, vma::memory_usage memory_usage)
        : device(device)
    {
        VkBuffer staging_buffer;
        VmaAllocation staging_allocation;

        // Create the staging buffer.
        {
            vk::BufferCreateInfo staging_buffer_info({}, size, vk::BufferUsageFlagBits::eTransferSrc);
            VmaAllocationCreateInfo staging_alloc_info = { 0, VMA_MEMORY_USAGE_CPU_ONLY };

            VkBufferCreateInfo staging_info = static_cast<VkBufferCreateInfo>(staging_buffer_info);

            vmaCreateBuffer(device->get_vma_allocator(),
                &staging_info,
                &staging_alloc_info,
                &staging_buffer,
                &staging_allocation,
                nullptr);

            void *staging_data;
            vmaMapMemory(device->get_vma_allocator(), staging_allocation, &staging_data);
            memcpy(staging_data, data, size);
            vmaUnmapMemory(device->get_vma_allocator(), staging_allocation);
        }

        // Create the device buffer.
        {
            vk::BufferCreateInfo device_buffer_info({}, size, usage | vk::BufferUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive);
            VmaAllocationCreateInfo device_alloc_info = { 0, vma::to_vma_memory_usage(memory_usage) };

            VkBufferCreateInfo device_info = static_cast<VkBufferCreateInfo>(device_buffer_info);
            VkBuffer buffer;

            vmaCreateBuffer(device->get_vma_allocator(),
                &device_info,
                &device_alloc_info,
                &buffer,
                &this->allocation,
                nullptr);

            this->vk_buffer = buffer;
        }

        // Copy data from the staging buffer to the device buffer.
        {
            vk::CommandBuffer buffer = commands->start_small_buffer();
            commands->begin(buffer);

            vk::BufferCopy copy_region(0, 0, size);

            buffer.copyBuffer(staging_buffer, this->vk_buffer, 1, &copy_region);
            commands->submit_and_wait(buffer);
        }

        vmaDestroyBuffer(device->get_vma_allocator(), staging_buffer, staging_allocation);
    }

    template<typename T>
    buffer<T>::~buffer()
    {
        vmaDestroyBuffer(device->get_vma_allocator(), this->vk_buffer, this->allocation);
    }
}