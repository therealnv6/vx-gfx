#include "config.h"
#include <buffer/buffer.h>
#include <device.h>
#include <stdexcept>
#include <type_traits>
#include <util.h>
#include <vertex.h>
#include <vulkan/vulkan_core.h>

namespace gfx
{

	template<>
	class buffer<VkImage>
	{
	public:
		buffer(std::shared_ptr<gfx::device> device, std::shared_ptr<gfx::commands> commands, VkImage image, vk::ImageUsageFlags usage, vma::memory_usage memory_usage);
		~buffer();

	private:
		void create_device_image(vk::ImageUsageFlags usage, vma::memory_usage memory_usage);
		void destroy_image();

		std::shared_ptr<gfx::device> device;
		std::shared_ptr<gfx::commands> commands;

		VkImage image;
		VmaAllocation allocation;
	};

	buffer<VkImage>::buffer(std::shared_ptr<gfx::device> device, std::shared_ptr<gfx::commands> commands, VkImage image, vk::ImageUsageFlags usage, vma::memory_usage memory_usage)
		: device(device)
		, commands(commands)
		, image(image)
	{
		create_device_image(usage, memory_usage);
	}

	buffer<VkImage>::~buffer()
	{
		destroy_image();
	}

	void buffer<VkImage>::create_device_image(vk::ImageUsageFlags usage, vma::memory_usage memory_usage)
	{
		vk::ImageCreateInfo image_info {
			{},
			vk::ImageType::e2D,
			vk::Format::eUndefined,
			vk::Extent3D(1, 1, 1),
			1,
			1,
			vk::SampleCountFlagBits::e1,
			vk::ImageTiling::eOptimal,
			usage,
			vk::SharingMode::eExclusive,
		};

		VmaAllocationCreateInfo alloc_info = { 0, vma::to_vma_memory_usage(memory_usage) };
		VmaAllocation allocation;

		VkImageCreateInfo device_info = static_cast<VkImageCreateInfo>(image_info);

		vmaCreateImage(device->get_vma_allocator(),
			&device_info,
			&alloc_info,
			&image,
			&allocation,
			nullptr);
	}

	void buffer<VkImage>::destroy_image()
	{
		vmaDestroyImage(device->get_vma_allocator(), image, allocation);
	}
}
