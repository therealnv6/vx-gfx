#pragma once

#include <context.h>
#include <string>
#include <vector>

namespace gfx
{
    static const std::vector<vk::DynamicState> dynamic_states = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    std::vector<char> read_file(const std::string &file_path);

    class pipeline
    {
    public:
        vk::PipelineLayout pipeline_layout;

        pipeline(gfx::context &context, const std::string &vertex_file_path, const std::string &fragment_file_path);
        vk::ShaderModule create_shader_module(gfx::context &context, const std::vector<char> &code);

    private:
        void create_graphics_pipeline(gfx::context &context, const std::string &vertex_file_path, const std::string &fragment_file_path);
    };
}