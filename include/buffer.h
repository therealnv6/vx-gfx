#pragma once

#include <memory>
#include <vector>
#include <vertex.h>
#include <vulkan/vulkan.hpp>

namespace gfx
{
    class device;

    // A buffer that stores vertices for use in rendering.
    template<typename T, uint32_t SIZE>
    class vertex_buffer
    {
    public:
        // Constructor. Takes a shared pointer to a device object and a vector of elements.
        vertex_buffer(std::shared_ptr<gfx::device> device, const std::vector<T> &elements);

        // Destructor.
        ~vertex_buffer();

        // Returns the Vulkan buffer object associated with this vertex buffer.
        vk::Buffer get_buffer() const
        {
            return this->buffer;
        }

        // Finds the memory type that can be used to allocate the buffer.
        uint32_t find_memory_type(gfx::device &device, uint32_t type_filter, vk::MemoryPropertyFlags properties);

    private:
        std::shared_ptr<gfx::device> device; // Shared pointer to the device object.
        vk::Buffer buffer; // Vulkan buffer object.
        vk::DeviceMemory memory; // Device memory for the buffer.
    };

    // Instantiate the vertex_buffer template with the vertex class and a size of 3.
    template class gfx::vertex_buffer<gfx::vertex, 3u>;
}
