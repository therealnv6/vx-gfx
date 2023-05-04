#pragma once

#include <functional>
#include <global.h>
#include <stdexcept>

namespace gfx
{
    class context;

    class renderer
    {
        typedef std::function<void(vk::CommandBuffer *, uint32_t)> command_buffer_record_type;
        typedef std::function<vk::CommandBufferBeginInfo(gfx::context &context)> begin_type;

    public:
        void draw(command_buffer_record_type record, begin_type begin_command);

    private:
        context *context;

        command_buffer_record_type record = [](vk::CommandBuffer *buffer, uint32_t image_index)
        {
            throw std::runtime_error("renderer->record must be initialized!");
        };

        begin_type begin_command = [](gfx::context &context)
        {
            throw std::runtime_error("renderer->begin_command must be initialized!");
            return vk::CommandBufferBeginInfo {};
        };
    };
}