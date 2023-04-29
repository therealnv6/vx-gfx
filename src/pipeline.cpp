#include <fstream>
#include <iostream>
#include <pipeline.h>

namespace gfx
{
    vk::ShaderModule pipeline::create_shader_module(gfx::context &context, std::vector<unsigned int> code)
    {
        vk::ShaderModuleCreateInfo shader_create(vk::ShaderModuleCreateFlags(), code);
        vk::ShaderModule shader_module = context.device->createShaderModule(shader_create);

        return shader_module;
    }

    pipeline::pipeline(gfx::context &context, const std::string &vertex_file_path, const std::string &fragment_file_path)
    {
        this->create_graphics_pipeline(context, vertex_file_path, fragment_file_path);
    }

    std::vector<unsigned int> read_file(const std::string &file_path)
    {
        std::ifstream is;
        std::vector<unsigned int> buffer;

        is.open(file_path, std::ios::binary);
        is.seekg(0, std::ios::end);

        size_t filesize = is.tellg();
        is.seekg(0, std::ios::beg);

        buffer.resize(filesize / sizeof(unsigned int));

        is.read((char *) buffer.data(), filesize);

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

        vk::PipelineRasterizationStateCreateInfo rasterizer({},
            false, // depthClampEnable
            false, // rasterizerDiscardEnable
            vk::PolygonMode::eFill, // polygonMode
            vk::CullModeFlagBits::eBack, // cullMode
            vk::FrontFace::eClockwise, // frontFace
            false, // depthBiasEnable
            0.0f, // depthBiasConstantFactor
            0.0f, // depthBiasClamp
            0.0f // depthBiasSlopeFactor
        );

        vk::PipelineMultisampleStateCreateInfo multisampling({}, vk::SampleCountFlagBits::e1, false);
        vk::PipelineColorBlendAttachmentState color_blend_attachment(true, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd);
        color_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR
            | vk::ColorComponentFlagBits::eG
            | vk::ColorComponentFlagBits::eB
            | vk::ColorComponentFlagBits::eA;

        color_blend_attachment.blendEnable = false;

        vk::PipelineColorBlendStateCreateInfo colorBlending(
            {}, // flags
            VK_FALSE, // logicOpEnable
            vk::LogicOp::eCopy, // logicOp (optional)
            1, // attachmentCount
            &color_blend_attachment, // pAttachments
            { 0.0f, 0.0f, 0.0f, 0.0f } // blendConstants (optional)
        );

        vk::PipelineLayoutCreateInfo pipeline_layout_info;
        this->pipeline_layout = context.device->createPipelineLayout(pipeline_layout_info);
    }
}