#include <buffer.h>
#include <device.h>
#include <vertex.h>
#include <vulkan/vulkan_core.h>

namespace gfx
{
    template<typename T>
    buffer<T>::buffer(std::shared_ptr<gfx::device> device, const std::vector<T> &data, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
        : device(device)
    {
        // Create the buffer.
        vk::BufferCreateInfo buffer_create_info({}, sizeof(T) * data.size(), usage);
        vk_buffer = device->get_logical_device().createBuffer(buffer_create_info);

        // Allocate memory for the buffer.
        vk::MemoryRequirements mem_reqs = device->get_logical_device().getBufferMemoryRequirements(vk_buffer);
        vk::MemoryAllocateInfo mem_alloc_info(mem_reqs.size, find_memory_type(device->get_physical_device(), mem_reqs.memoryTypeBits, properties));
        memory = device->get_logical_device().allocateMemory(mem_alloc_info);

        // Bind the memory to the buffer.
        device->get_logical_device().bindBufferMemory(vk_buffer, memory, 0);

        // Copy data to the buffer if it is provided.
        if (!data.empty())
        {
            void *mapped_data = device->get_logical_device().mapMemory(memory, 0, sizeof(T) * data.size());
            memcpy(mapped_data, data.data(), sizeof(T) * data.size());
            device->get_logical_device().unmapMemory(memory);
        }
    }

    template<typename T>
    buffer<T>::~buffer()
    {
        device->get_logical_device().destroyBuffer(vk_buffer);
        device->get_logical_device().freeMemory(memory);
    }

    template<typename T>
    uint32_t buffer<T>::find_memory_type(vk::PhysicalDevice physical_device, uint32_t type_filter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties mem_properties = physical_device.getMemoryProperties();

        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
        {
            if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type for buffer.");
    }
    template<typename T>
    vma_buffer<T>::vma_buffer(std::shared_ptr<gfx::device> device, std::shared_ptr<gfx::commands> commands, const std::vector<T> &data, vk::BufferUsageFlags usage, VmaMemoryUsage vma_usage)
        : device(device)
    {
        auto size = sizeof(T) * data.size();

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
            memcpy(staging_data, data.data(), size);
            vmaUnmapMemory(device->get_vma_allocator(), staging_allocation);
        }

        // Create the device buffer.
        {
            vk::BufferCreateInfo device_buffer_info({}, size, usage | vk::BufferUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive);
            VmaAllocationCreateInfo device_alloc_info = { 0, vma_usage };

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
    vma_buffer<T>::~vma_buffer()
    {
        vmaDestroyBuffer(device->get_vma_allocator(), this->vk_buffer, this->allocation);
    }
}