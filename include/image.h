#pragma once
#include <../thirdparty/stb_image.h> // we still have to fix this.
#include <buffer/buffer.h>
#include <device.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_enums.hpp>

namespace img
{
	class texture_image
	{
	public:
		texture_image(std::shared_ptr<gfx::device> device, std::shared_ptr<gfx::commands> commands, std::string path)
			: device(device)
			, commands(commands)
			, path(path)
		{
			create_image();
			create_buffer();
		}

		~texture_image()
		{
			vmaDestroyImage(device->get_vma_allocator(), image, image_allocation);
		}

		void create_buffer()
		{
			stbi_uc *pixels = stbi_load(path.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

			if (!pixels)
			{
				throw std::runtime_error("Unable to load texture image.");
			}

			vk::DeviceSize size = tex_width * tex_height * 4 /* 4 for 4 channels (r, g, b, a)*/;
			vk::BufferUsageFlags flags = vk::BufferUsageFlagBits::eVertexBuffer
				| vk::BufferUsageFlagBits::eTransferDst; // we want to have eTransferDst for staging buffer.

			buffer = std::make_unique<gfx::buffer<stbi_uc *>>(device, commands, pixels, size, flags, vma::memory_usage::CpuOnly);

			// Copy the pixel data from the staging buffer to the image buffer.
			{
				vk::CommandBuffer command_buffer = commands->start_small_buffer();
				commands->begin(command_buffer);

				vk::ImageMemoryBarrier barrier({}, vk::AccessFlagBits::eTransferWrite, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
				vk::BufferImageCopy region(0, 0, 0, vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1), vk::Offset3D(0, 0, 0), vk::Extent3D(tex_width, tex_height, 1));

				// Transition the image layout to transfer destination.
				command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, { barrier });

				// Copy the buffer to the image.
				command_buffer.copyBufferToImage(buffer->get_buffer(), image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

				// Transition the image layout to shader read-only.
				barrier = vk::ImageMemoryBarrier(vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
				command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, { barrier });

				commands->submit_and_wait(command_buffer);
			}
		}

		void create_image()
		{
			int channels = 4; // RGBA

			vk::Extent3D extent(tex_width, tex_height, 1);
			vk::Format format = vk::Format::eR8G8B8A8Srgb;
			vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;

			// Create the image.
			vk::ImageCreateInfo info({}, vk::ImageType::e2D, format, extent, 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, usage, vk::SharingMode::eExclusive, 0, nullptr, vk::ImageLayout::eUndefined);
			VkImageCreateInfo image_info = static_cast<VkImageCreateInfo>(info);

			VmaAllocationCreateInfo allocation_create_info = {};
			allocation_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

			if (vmaCreateImage(device->get_vma_allocator(), &image_info, &allocation_create_info, &image, &image_allocation, nullptr) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create image.");
			}
		}

	private:
		std::shared_ptr<gfx::device> device;
		std::shared_ptr<gfx::commands> commands;

		std::unique_ptr<gfx::buffer<stbi_uc *>> buffer;

		std::string path;

		int tex_width;
		int tex_height;
		int tex_channels;

		VkImage image;
		VmaAllocation image_allocation;
	};
}
