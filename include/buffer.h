#pragma once

#include <commands.h>
#include <memory>
#include <util.h>
#include <vector>
#include <vertex.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

namespace gfx
{
    class device;

    template<typename T>
    class buffer
    {
    public:
        buffer(std::shared_ptr<gfx::device> device, const std::vector<T> &data, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
        ~buffer();

        vk::Buffer get_buffer() const
        {
            return this->vk_buffer;
        }

    private:
        std::shared_ptr<gfx::device> device;
        vk::Buffer vk_buffer;
        vk::DeviceMemory memory;

        uint32_t find_memory_type(vk::PhysicalDevice physical_device, uint32_t type_filter, vk::MemoryPropertyFlags properties);
    };

    template class buffer<gfx::vertex>;

    template<typename T>
    class vma_buffer
    {
    public:
        vma_buffer(std::shared_ptr<gfx::device> device, std::shared_ptr<gfx::commands> commands, const std::vector<T> &data, vk::BufferUsageFlags usage, vma::memory_usage memory_usage);
        ~vma_buffer();

        vk::Buffer get_buffer() const
        {
            return this->vk_buffer;
        }

    private:
        std::shared_ptr<gfx::device> device;

        vk::Buffer vk_buffer;
        VmaAllocation allocation;
    };

    template class vma_buffer<gfx::vertex>;
}
