#pragma once
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

		static std::vector<vk::VertexInputAttributeDescription> get_attribute_descriptions()
		{
			return std::vector<vk::VertexInputAttributeDescription> {
				vk::VertexInputAttributeDescription {0, 0,	vk::Format::eR32G32Sfloat, offsetof(gfx::vertex,	 pos)},
				vk::VertexInputAttributeDescription {1, 0, vk::Format::eR32G32B32Sfloat, offsetof(gfx::vertex, color)}
			};
		}
	};
}
