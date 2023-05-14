#pragma once
#include <buffer/buffer.h>

namespace gfx
{
    template<class T>
    class uniform_buffer : public buffer<T>
    {
    public:
        uniform_buffer(std::shared_ptr<gfx::device> device, std::shared_ptr<gfx::commands> commands, T &data, size_t size, vma::memory_usage memory_usage)
            : buffer<T>(device, commands, data, size, vk::BufferUsageFlagBits::eUniformBuffer, memory_usage)
        {
        }

        ~uniform_buffer() {

        };
    };

    template class gfx::uniform_buffer<uniform_buffer_object>;
}
