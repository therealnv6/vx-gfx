#include <buffer.h>
#include <device.h>
#include <vertex.h>

namespace gfx
{
    template<typename T, uint32_t SIZE>
    vertex_buffer<T, SIZE>::vertex_buffer(gfx::device *device, const std::vector<T> &elements)
    {
        vk::DeviceSize buffer_size = sizeof(T) * SIZE;
        vk::BufferCreateInfo buffer_create_info({},
            buffer_size,
            vk::BufferUsageFlagBits::eVertexBuffer,
            vk::SharingMode::eExclusive);

        this->buffer = device->logical_device.createBuffer(buffer_create_info);

        vk::MemoryRequirements memory_requirements = device->logical_device.getBufferMemoryRequirements(buffer);
        vk::MemoryAllocateInfo allocate_info(memory_requirements.size, this->find_memory_type(device, memory_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

        this->memory = device->logical_device.allocateMemory(allocate_info);

        device->logical_device.bindBufferMemory(buffer, memory, 0);
        void *data = device->logical_device.mapMemory(memory, 0, buffer_size);
        memcpy(data, elements.data(), static_cast<size_t>(buffer_size));

        device->logical_device.unmapMemory(memory);
    }

    template<typename T, uint32_t SIZE>
    vertex_buffer<T, SIZE>::~vertex_buffer()
    {
        device->logical_device.destroyBuffer(buffer);
        device->logical_device.freeMemory(memory);
    }

    template<typename T, uint32_t SIZE>
    uint32_t vertex_buffer<T, SIZE>::find_memory_type(gfx::device *device, uint32_t type_filter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDevice physical_device = device->physical_device;
        vk::PhysicalDeviceMemoryProperties mem_properties = physical_device.getMemoryProperties();

        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
        {
            if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

}