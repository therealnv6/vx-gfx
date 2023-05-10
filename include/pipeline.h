#pragma once
#include "swapchain.h"
#include <device.h>
#include <string>
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
        gfx::render_pass *pass;

        std::vector<vk::DynamicState> dynamic_states = {
            vk::DynamicState::eViewport, // The dynamic viewport state.
            vk::DynamicState::eScissor, // The dynamic scissor state.
        };

        std::vector<vk::VertexInputBindingDescription> binding_descriptions;
        std::vector<vk::VertexInputAttributeDescription> attribute_descriptions;

        // The constructor for the pipeline class.
        pipeline(gfx::swapchain *swapchain,
            const std::string &parent_pass,
            const std::string vert_shader_name,
            const std::string frag_shader_name);

        // The destructor for the pipeline class.
        ~pipeline()
        {
            this->cleanup();
        }

        void initialize();

        template<typename T>
        void bind();

        template<typename T>
        void bind_struct(vk::VertexInputBindingDescription binding, std::vector<vk::VertexInputAttributeDescription>);

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
        gfx::device *device; // A pointer to the device object.
        gfx::swapchain *swapchain; // A pointer to the swapchain object.
    };
}