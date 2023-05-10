#pragma once
#include <vector>
#include <vertex.h>
#include <vulkan/vulkan.hpp>

namespace gfx
{
    class device; // forward declaration, from device.h

    template<typename T, uint32_t SIZE>
    class vertex_buffer
    {
    public:
        vertex_buffer(gfx::device *device, const std::vector<T> &elements);
        ~vertex_buffer();

        vk::Buffer inner() const
        {
            return this->buffer;
        }

        uint32_t find_memory_type(gfx::device *device, uint32_t type_filter, vk::MemoryPropertyFlags properties);

    private:
        gfx::device *device;
        vk::Buffer buffer;
        vk::DeviceMemory memory;
    };

    template class gfx::vertex_buffer<gfx::vertex, 3u>;
}