#pragma once
#include <buffer/buffer.h>

namespace gfx
{
    template<typename T>
    class uniform_buffer : public buffer<T>
    {
    public:
        uniform_buffer(std::shared_ptr<gfx::device> device, std::shared_ptr<gfx::commands> commands, const T &data, size_t size, vma::memory_usage memory_usage, vk::IndexType type)
            : buffer<T>(device, commands, data, size, vk::BufferUsageFlagBits::eIndexBuffer, memory_usage)
            , type { type }
        {
        }

        ~uniform_buffer() {

        };

        vk::IndexType get_index_type()
        {
            return this->type;
        }

    private:
        vk::IndexType type;
    };

    template class uniform_buffer<uint16_t>;
    template class uniform_buffer<uint32_t>;
}
