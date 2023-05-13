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
        buffer(std::shared_ptr<gfx::device> device, std::shared_ptr<gfx::commands> commands, const T &data, size_t size, vk::BufferUsageFlags usage, vma::memory_usage memory_usage);
        ~buffer();

        vk::Buffer get_buffer() const
        {
            return this->vk_buffer;
        }

    private:
        std::shared_ptr<gfx::device> device;

        vk::Buffer vk_buffer;
        VmaAllocation allocation;
    };

    template class gfx::buffer<const gfx::vertex *>;

    template class buffer<const uint16_t *>;
    template class buffer<const uint32_t *>;

    template<typename T>
    class vec_buffer : public buffer<const T *>
    {
    public:
        vec_buffer(std::shared_ptr<gfx::device> device, std::shared_ptr<gfx::commands> commands, const std::vector<T> data, vk::BufferUsageFlags usage, vma::memory_usage memory_usage)
            : gfx::buffer<const T *> {
                device,
                commands,
                data.data(),
                sizeof(T) * data.size(),
                usage,
                memory_usage
            } {};
        ~vec_buffer() {};

        vk::Buffer get_buffer() const
        {
            return this->vk_buffer;
        }

    private:
        std::shared_ptr<gfx::device> device;
        vk::Buffer vk_buffer;

        VmaAllocation allocation;
    };

    template class gfx::vec_buffer<gfx::vertex>;
}
