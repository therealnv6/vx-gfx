#pragma once
#include "uniform/layout.h"
#include <buffer/buffer.h>
#include <buffer/index.h>
#include <device.h>
#include <string>
#include <swapchain/swapchain.h>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace gfx
{
    // This function reads a file and returns its contents as a vector of characters.
    static std::vector<char> read_file(const std::string &file_name);

    // This class represents a graphics pipeline in Vulkan.
    class pipeline
    {
    public:
        vk::Pipeline vk_pipeline; // The Vulkan pipeline handle.
        vk::PipelineLayout pipeline_layout;

        std::shared_ptr<gfx::render_pass> pass;

        std::vector<vk::DynamicState> dynamic_states = {
            vk::DynamicState::eViewport, // The dynamic viewport state.
            vk::DynamicState::eScissor, // The dynamic scissor state.
        };

        std::vector<vk::VertexInputBindingDescription> binding_descriptions;
        std::vector<vk::VertexInputAttributeDescription> attribute_descriptions;
        std::vector<vk::DescriptorSetLayout> layouts;

        // The constructor for the pipeline class.
        pipeline(std::shared_ptr<gfx::swapchain> swapchain,
            const std::string &parent_pass,
            const std::string vert_shader_name,
            const std::string frag_shader_name);

        // The destructor for the pipeline class.
        ~pipeline()
        {
            this->cleanup();
        }

        void initialize();

        void bind_uniform_layout(gfx::uniform_layout layout);

        template<class T>
        void bind_vertex_buffer(vk::VertexInputBindingDescription binding, std::vector<vk::VertexInputAttributeDescription>);

        template<class T>
        void bind(vk::CommandBuffer *buffer,
            vk::ArrayProxy<vk::Buffer> buffers = {},
            vk::ArrayProxy<std::reference_wrapper<gfx::index_buffer<T>>> index_buffers = {},
            vk::ArrayProxy<vk::DescriptorSet> descriptor_sets = {},
            vk::ArrayProxy<const vk::DeviceSize> const &offsets = { 0 });

        // This function cleans up the pipeline and releases any allocated resources.
        void cleanup();

        // This function creates a Vulkan shader module from the provided code.
        vk::ShaderModule create_shader_module(
            const std::vector<char> &code);

        // This function creates the graphics pipeline using the provided vertex and fragment shader names.
        void create_graphics_pipeline();

    private:
        const std::string vert_shader_name;
        const std::string frag_shader_name;
        std::shared_ptr<gfx::device> device; // A pointer to the device object.
        std::shared_ptr<gfx::swapchain> swapchain; // A pointer to the swapchain object.
    };
}