#pragma once
#include <buffer/uniform.h>
#include <config.h>
#include <device.h>
#include <stdexcept>
#include <uniform/layout.h>
#include <uniform/pool.h>
#include <vulkan/vulkan.hpp>

namespace gfx
{
	template<class T>
	class descriptor_set
	{
	public:
		descriptor_set(std::shared_ptr<gfx::descriptor_pool> pool, gfx::uniform_layout layout, gfx::uniform_buffer<T> &uniform_buffer)
			: sets { pool->create_descriptor_sets(layout) }
			, uniform_buffer { uniform_buffer }
			, device { pool->device }
		{
			this->update_descriptor_sets();
		}

		std::vector<vk::DescriptorSet> sets;

	private:
		gfx::uniform_buffer<T> &uniform_buffer;
		std::shared_ptr<gfx::device> device;

		void update_descriptor_sets()
		{
			for (uint32_t i = 0; i < uniform_buffer.buffers.size(); i++)
			{
				auto buffer = uniform_buffer.get_buffer(i);

				vk::DescriptorBufferInfo buffer_info { buffer, 0, uniform_buffer.size };
				vk::WriteDescriptorSet write_descriptor_set {
					this->sets[i],
					0,
					0,
					1,
					vk::DescriptorType::eUniformBuffer,
					nullptr,
					&buffer_info
				};

				device->get_logical_device().updateDescriptorSets(1, &write_descriptor_set, 0, nullptr);
			}
		}
	};
}
