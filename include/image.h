#pragma once
#include "util.h"
#include <buffer/buffer.h>
#include <device.h>
#include <optional>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace gfx
{
	// considering we won't be using any kinds of textures, just a depth buffer class is enough.
	// if anyone ever sees this and would like to use their own textures instead of just voxel colours, feel free to
	// change this code.
	class depth
	{
	public:
		depth(std::shared_ptr<gfx::device> device, std::shared_ptr<gfx::commands> commands, vk::Format format)
			: device(device)
			, commands(commands)
		{
			this->create_image();
			this->create_image_view(format);
		}

		void create_image()
		{
			this->buffer = std::make_unique<gfx::buffer<VkImage>>(
				device, commands, vk::ImageUsageFlagBits::eDepthStencilAttachment, vma::memory_usage::GpuOnly //
			);
		}

		void create_image_view(vk::Format format)
		{
			vk::ImageViewCreateInfo viewInfo(
				vk::ImageViewCreateFlags(),
				buffer->get_image(),
				vk::ImageViewType::e2D,
				format,
				vk::ComponentMapping(),
				vk::ImageSubresourceRange(
					vk::ImageAspectFlagBits::eColor,
					0,
					1,
					0,
					1 //
					) //
			);

			view = device->get_logical_device().createImageView(viewInfo);
		}

	private:
		std::shared_ptr<gfx::device> device;
		std::shared_ptr<gfx::commands> commands;

		std::unique_ptr<gfx::buffer<VkImage>> buffer;

		vk::ImageView view;
	};
}
