#pragma once

#include <context.h>
#include <string>
#include <vector>
#include <renderpass.h>

namespace gfx
{
    static const std::vector<vk::DynamicState> dynamic_states = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    std::vector<unsigned int> read_file(const std::string &file_path);

    class pipeline
    {
    public:
        vk::Pipeline graphics_pipeline; 
        gfx::render_pass render_pass;
        std::shared_ptr<gfx::context> context;
        
        pipeline(std::shared_ptr<gfx::context> context, const std::string &vertex_file_path, const std::string &fragment_file_path);
        ~pipeline();

        vk::ShaderModule create_shader_module(gfx::context &context, std::vector<unsigned int>);

    private:
        void create_graphics_pipeline(std::shared_ptr<gfx::context> context, const std::string &vertex_file_path, const std::string &fragment_file_path);
    };
}