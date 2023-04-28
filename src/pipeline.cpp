#include <fstream>
#include <iostream>
#include <pipeline.h>

namespace gfx
{
    vk::ShaderModule pipeline::create_shader_module(gfx::context &context, const std::vector<char> &code)
    {
        vk::ShaderModuleCreateInfo shader_create(vk::ShaderModuleCreateFlagBits {}, code.size(), reinterpret_cast<const uint32_t *>(code.data()));
        vk::ShaderModule shader_module = context.device->createShaderModule(shader_create);

        return shader_module;
    }

    pipeline::pipeline(gfx::context &context, const std::string &vertex_file_path, const std::string &fragment_file_path)
    {
        this->create_graphics_pipeline(context, vertex_file_path, fragment_file_path);
    }

    std::vector<char> read_file(const std::string &file_path)
    {
        std::ifstream file { file_path, std::ios::ate | std::ios::binary };

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file: " + file_path);
        }

        size_t file_size = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(file_size);

        file.seekg(0);
        file.read(buffer.data(), file_size);

        file.close();
        return buffer;
    }

    void pipeline::create_graphics_pipeline(gfx::context &context, const std::string &vertex_file_path, const std::string &fragment_file_path)
    {
        auto vertex_code = gfx::read_file(vertex_file_path);
        auto fragment_code = gfx::read_file(fragment_file_path);

        vk::ShaderModule vertex_shader = this->create_shader_module(context, vertex_code);
        vk::ShaderModule fragment_shader = this->create_shader_module(context, fragment_code);

        vk::PipelineShaderStageCreateInfo vertex_shader_stage_info({},
            vk::ShaderStageFlagBits::eVertex, // stage
            vertex_shader, // module
            "main" // pName
        );

        vk::PipelineShaderStageCreateInfo fragment_shader_stage_info({},
            vk::ShaderStageFlagBits::eFragment, // stage
            fragment_shader, // module
            "main" // pName
        );

        vk::PipelineVertexInputStateCreateInfo vertex_input_info({},
            0, // vertexBindingDescriptionCount
            nullptr, // vertexBindingDescriptions
            0, // vertexAttributeDescriptionCount
            nullptr // pVertexAttributeDescriptions
        );

        vk::PipelineInputAssemblyStateCreateInfo input_assembly_info({},
            vk::PrimitiveTopology::eTriangleList, // topology
            false // primitiveRestartEnable
        );

        vk::Viewport viewport(
            0.0f, // x
            0.0f, // y
            (float) context.swap_chain_extent.width, // width
            (float) context.swap_chain_extent.height, // height
            0.0f, // minDepth
            1.0f // maxDepth
        );

        vk::Rect2D scissor({ 0, 0 }, context.swap_chain_extent);

        vk::PipelineDynamicStateCreateInfo dynamic_state_info({},
            static_cast<uint32_t>(gfx::dynamic_states.size()), // dynamicStateCount
            gfx::dynamic_states.data() // pDynamicStates
        );

        vk::PipelineViewportStateCreateInfo view_port_state_info({},
            1, // viewportCount
            &viewport, // pViewports
            1, // scissorCount
            &scissor // pScissors
        );
    }
}