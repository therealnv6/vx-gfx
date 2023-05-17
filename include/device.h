#pragma once
#include <functional>
#include <global.h>
#include <optional>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>

namespace gfx
{
	static const std::vector<const char *> device_extensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME,
		VK_KHR_MAINTENANCE2_EXTENSION_NAME,
		VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME,
		// VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME,
	};

	class device
	{
	public:
		vk::Queue graphics_queue;
		vk::Queue present_queue;

		VmaAllocator allocator;

		const vk::QueueFlags queue_flags = vk::QueueFlagBits::eGraphics;

		// Function for checking if a physical device is suitable for use.
		// This function takes a physical device as input and returns a boolean value.
		std::function<bool(vk::PhysicalDevice)> device_suitable = [](vk::PhysicalDevice device) {
			// Default is that all devices are suitable for use.
			return true;
		};

		device(const vk::Instance *instance, const vk::SurfaceKHR *surface);
		~device();

		device(const device &) = delete;
		device operator=(const device &) = delete;

		gfx::queue_family_indices find_queue_families(const vk::SurfaceKHR *surface, std::optional<vk::PhysicalDevice> device);

		VmaAllocator get_vma_allocator()
		{
			return allocator;
		}

		vk::Device get_logical_device()
		{
			return logical_device;
		}

		vk::PhysicalDevice get_physical_device()
		{
			return physical_device;
		}

	private:
		float queue_priority = 1.0f;

		vk::Device logical_device;
		vk::PhysicalDevice physical_device;

		std::optional<std::pair<vk::PhysicalDevice, gfx::queue_family_indices>> find_most_suitable(
			const std::vector<vk::PhysicalDevice> device,
			const vk::SurfaceKHR *surface);

		uint32_t evaluate_device(vk::PhysicalDevice physical_device, gfx::queue_family_indices indices);

		void cleanup();
		void init_vma(const vk::Instance *instance);
	};
}
