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

    template<typename T>
    class vma_index_buffer : public vma_buffer<T>
    {
    public:
        vma_index_buffer(std::shared_ptr<gfx::device> device, std::shared_ptr<gfx::commands> commands, const std::vector<T> &data, vma::memory_usage memory_usage, vk::IndexType type)
            : vma_buffer<T>(device, commands, data, vk::BufferUsageFlagBits::eIndexBuffer, memory_usage)
            , type { type }
        {
        }

        ~vma_index_buffer()
        {
            
        };

        vk::IndexType get_index_type()
        {
            return this->type;
        }

    private:
        vk::IndexType type;
    };

    template class vma_buffer<gfx::vertex>;
    template class vma_buffer<uint16_t>;
    template class vma_buffer<uint32_t>;

    template class vma_index_buffer<uint16_t>;
    template class vma_index_buffer<uint32_t>;
}
