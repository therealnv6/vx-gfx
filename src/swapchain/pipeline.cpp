#include "vertex.h"
#include <buffer/buffer.h>
#include <fstream>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <swapchain/pipeline.h>
#include <swapchain/swapchain.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace gfx
{
	// taken from https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Shader_modules
	// will probably rewrite this eventually
	static std::vector<char> read_file(const std::string &file_name)
	{
		std::ifstream file(file_name, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		size_t file_size = (size_t) file.tellg();
		std::vector<char> buffer(file_size);

		file.seekg(0);
		file.read(buffer.data(), file_size);

		file.close();

		return buffer;
	}

	pipeline::pipeline(std::shared_ptr<gfx::swapchain> swapchain,
		const std::string &parent_pass,
		const std::string vert_shader_name,
		const std::string frag_shader_name)
		: swapchain { swapchain }
		, device { swapchain->device }
		, pass { &swapchain->render_passes.at(parent_pass) }
		, vert_shader_name { vert_shader_name }
		, frag_shader_name { frag_shader_name }
	{
	}

	void pipeline::cleanup()
	{
		spdlog::info("cleaning up gfx::pipeline");
		device->get_logical_device().destroyPipeline(vk_pipeline);
		spdlog::info("... done!");
	}

	vk::ShaderModule pipeline::create_shader_module(const std::vector<char> &code)
	{
		vk::ShaderModuleCreateInfo create_info({}, code.size(), reinterpret_cast<const uint32_t *>(code.data()));
		vk::ShaderModule module = device->get_logical_device().createShaderModule(create_info);

		return module;
	}

	void pipeline::initialize()
	{
		this->create_graphics_pipeline();
	}

	void pipeline::bind_uniform_layout(gfx::uniform_layout layout)
	{
		this->layouts.push_back(layout.layout);
	}

	template<class T>
	void pipeline::bind_vertex_buffer(vk::VertexInputBindingDescription binding, std::vector<vk::VertexInputAttributeDescription> attributes)
	{
		this->binding_descriptions.push_back(binding);

		for (auto attribute : attributes)
		{
			this->attribute_descriptions.push_back(attribute);
		}
	}

	template<class T>
	void pipeline::bind(vk::CommandBuffer *buffer,
		vk::ArrayProxy<vk::Buffer> buffers,
		vk::ArrayProxy<std::reference_wrapper<gfx::index_buffer<T>>> index_buffers,
		vk::ArrayProxy<vk::DescriptorSet> descriptor_sets,
		vk::ArrayProxy<const vk::DeviceSize> const &offsets)
	{
		if (!this->attribute_descriptions.empty() && buffers.empty())
		{
			spdlog::warn("pipeline::bind() was called without buffers, however attribute descriptions are present. did you forget to provide your buffers?");
		}

		buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, this->vk_pipeline);

		if (!buffers.empty())
		{
			buffer->bindVertexBuffers(0, buffers, offsets);
		}

		for (gfx::index_buffer<T> // we have to keep this as this type, not as auto. this will implicitly cast it from a std::reference_wrapper<T> to T itself.
				 &index_buffer : index_buffers)
		{
			buffer->bindIndexBuffer(index_buffer.get_buffer(), 0, index_buffer.get_index_type());
		}

		buffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
			pipeline_layout,
			0,
			static_cast<uint32_t>(descriptor_sets.size()),
			descriptor_sets.data(),
			0,
			nullptr);
	}

	template void pipeline::bind<const uint32_t *>(vk::CommandBuffer *buffer, vk::ArrayProxy<vk::Buffer> buffers, vk::ArrayProxy<std::reference_wrapper<gfx::index_buffer<const uint32_t *>>> index_buffers, vk::ArrayProxy<vk::DescriptorSet> descriptor_sets, vk::ArrayProxy<const vk::DeviceSize> const &offsets);
	template void pipeline::bind<const uint16_t *>(vk::CommandBuffer *buffer, vk::ArrayProxy<vk::Buffer> buffers, vk::ArrayProxy<std::reference_wrapper<gfx::index_buffer<const uint16_t *>>> index_buffers, vk::ArrayProxy<vk::DescriptorSet> descriptor_sets, vk::ArrayProxy<const vk::DeviceSize> const &offsets);
	template void pipeline::bind_vertex_buffer<gfx::vertex>(vk::VertexInputBindingDescription binding, std::vector<vk::VertexInputAttributeDescription> attributes);

	void pipeline::create_graphics_pipeline()
	{
		auto vert_code = gfx::read_file(vert_shader_name);
		auto frag_code = gfx::read_file(frag_shader_name);

		auto vertex_shader = this->create_shader_module(vert_code);
		auto fragment_shader = this->create_shader_module(frag_code);

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

		vk::PipelineShaderStageCreateInfo shader_stages[] = { fragment_shader_stage_info, vertex_shader_stage_info };
		vk::PipelineVertexInputStateCreateInfo vertex_input_info({},
			this->binding_descriptions.size(), // vertexBindingDescriptionCount
			this->binding_descriptions.data(), // vertexBindingDescriptions
			this->attribute_descriptions.size(), // vertexAttributeDescriptionCount
			this->attribute_descriptions.data() // pVertexAttributeDescriptions
		);

		vk::PipelineInputAssemblyStateCreateInfo input_assembly_info({},
			vk::PrimitiveTopology::eTriangleList, // topology
			false // primitiveRestartEnable
		);

		vk::Viewport viewport(
			0.0f, // x
			0.0f, // y
			(float) swapchain->extent.width, // width
			(float) swapchain->extent.height, // height
			0.0f, // minDepth
			1.0f // maxDepth
		);

		vk::Rect2D scissor({ 0, 0 }, swapchain->extent);
		vk::PipelineDynamicStateCreateInfo dynamic_state_info({},
			static_cast<uint32_t>(dynamic_states.size()), // dynamicStateCount
			dynamic_states.data() // pDynamicStates
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
			vk::FrontFace::eCounterClockwise, // frontFace
			false, // depthBiasEnable
			0.0f, // depthBiasConstantFactor
			0.0f, // depthBiasClamp
			0.0f, // depthBiasSlopeFactor
			1.0);

		vk::PipelineMultisampleStateCreateInfo multisampling({},
			vk::SampleCountFlagBits::e1,
			false);

		vk::PipelineColorBlendAttachmentState color_blend_attachment(true,
			vk::BlendFactor::eSrcAlpha,
			vk::BlendFactor::eOneMinusSrcAlpha,
			vk::BlendOp::eAdd,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eZero, vk::BlendOp::eAdd);

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

		vk::PipelineLayoutCreateInfo pipeline_layout_info {
			vk::PipelineLayoutCreateFlagBits::eIndependentSetsEXT,
			static_cast<uint32_t>(layouts.size()),
			this->layouts.data(),
		};

		this->pipeline_layout = device->get_logical_device().createPipelineLayout(pipeline_layout_info);

		vk::GraphicsPipelineCreateInfo pipelineInfo {
			{},
			sizeof(shader_stages) / sizeof(vk::PipelineShaderStageCreateInfo),
			shader_stages,
			&vertex_input_info,
			&input_assembly_info,
			nullptr,
			&view_port_state_info,
			&rasterizer,
			&multisampling,
			nullptr,
			&colorBlending,
			&dynamic_state_info,
			pipeline_layout,
		};

		pipelineInfo.subpass = 0;
		pipelineInfo.renderPass = pass->pass;
		pipelineInfo.basePipelineHandle = nullptr;
		pipelineInfo.basePipelineIndex = -1;

		vk::Result result;
		vk::Pipeline pipeline;

		std::tie(result, pipeline) = device->get_logical_device().createGraphicsPipeline(nullptr, pipelineInfo);

		if (result != vk::Result::eSuccess)
		{
			throw std::runtime_error("unable to make pipeline");
		}

		this->vk_pipeline = pipeline;

		device->get_logical_device().destroyShaderModule(vertex_shader);
		device->get_logical_device().destroyShaderModule(fragment_shader);
	}
}
