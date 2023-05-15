#pragma once
#include <buffer/buffer.h>

namespace gfx
{
    template<class T>
    class index_buffer : public buffer<T>
    {
    public:
        index_buffer(std::shared_ptr<gfx::device> device, std::shared_ptr<gfx::commands> commands, const T &data, size_t size, vma::memory_usage memory_usage, vk::IndexType type)
            : buffer<T>(device, commands, data, size, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, memory_usage)
            , type { type }
        {
        }

        ~index_buffer() {

        };

        vk::IndexType get_index_type()
        {
            return this->type;
        }

    private:
        vk::IndexType type;
    };

    template class index_buffer<const uint16_t *>;
    template class index_buffer<const uint32_t *>;
}
