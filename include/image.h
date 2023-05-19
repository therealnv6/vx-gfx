#pragma once
#include <buffer/buffer.h>
#include <device.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace gfx
{
	class image
	{
	public:
		image(std::shared_ptr<gfx::device> device, gfx::buffer<VkImage> &image)
			: buffer(image)
			, device(device)
		{
		}

		void create_image()
		{
		}

		void create_image_view(vk::Format format)
		{
			vk::ImageViewCreateInfo viewInfo(
				vk::ImageViewCreateFlags(),
				buffer.get_image(),
				vk::ImageViewType::e2D,
				format,
				vk::ComponentMapping(),
				vk::ImageSubresourceRange(
					vk::ImageAspectFlagBits::eColor,
					0,
					1,
					0,
					1));

			view = device->get_logical_device().createImageView(viewInfo);
		}

	private:
		std::shared_ptr<gfx::device> device;
		gfx::buffer<VkImage> &buffer;
		vk::ImageView view;
	};
}
