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

    template<class T>
    class buffer
    {
    public:
        size_t size;
        std::vector<vk::Buffer> buffers;

        buffer(std::shared_ptr<gfx::device> device, std::shared_ptr<gfx::commands> commands, const T &data, size_t size, vk::BufferUsageFlags usage, vma::memory_usage memory_usage);
        ~buffer();

        void *get_mapped_data(int frame = 0) const
        {
            return this->data_mapped[frame];
        }

        vk::Buffer get_buffer(int frame = 0) const
        {
            return this->buffers[frame];
        }

    private:
        std::shared_ptr<gfx::device> device;
        std::vector<VmaAllocation> allocations;
        std::vector<void *> data_mapped;
    };

    template class gfx::buffer<const gfx::vertex *>;

    template class buffer<const uint16_t *>;
    template class buffer<const uint32_t *>;

    struct uniform_buffer_object {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    // if you're using an object that is not yet registered as a template within any of the gfx::_buffer objects, you can do it like so:
    template class gfx::buffer<uniform_buffer_object>;

    template<class T>
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
    };

    template class gfx::vec_buffer<gfx::vertex>;
}
