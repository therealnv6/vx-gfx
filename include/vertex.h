#pragma once
#include <array>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

namespace gfx
{
    struct vertex {
        glm::vec2 pos;
        glm::vec3 color;

        static vk::VertexInputBindingDescription get_binding_description()
        {
            return vk::VertexInputBindingDescription {
                0,
                sizeof(gfx::vertex),
                vk::VertexInputRate::eVertex,
            };
        }

        static std::array<vk::VertexInputAttributeDescription, 2> get_attribute_descriptions()
        {
            return std::array<vk::VertexInputAttributeDescription, 2> {
                vk::VertexInputAttributeDescription {0, 0,    vk::Format::eR32G32Sfloat, offsetof(gfx::vertex,   pos)},
                vk::VertexInputAttributeDescription {0, 1, vk::Format::eR32G32B32Sfloat, offsetof(gfx::vertex, color)}
            };
        }
    };
}