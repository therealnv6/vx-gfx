#include "config.h"
#include <algorithm>
#include <buffer/buffer.h>
#include <device.h>
#include <stdexcept>
#include <type_traits>
#include <util.h>
#include <vertex.h>
#include <vulkan/vulkan_core.h>
namespace gfx
{
	template<class T>
	buffer<T>::buffer(std::shared_ptr<gfx::device> device, std::shared_ptr<gfx::commands> commands, const T &data, size_t size, vk::BufferUsageFlags usage, vma::memory_usage memory_usage)
		: device(device)
		, commands(commands)
		, size(size)
	{
		if (usage & vk::BufferUsageFlagBits::eTransferDst)
		{
			create_staging_buffer(data, size);
			create_device_buffer(usage | vk::BufferUsageFlagBits::eTransferDst, memory_usage);
			copy_data(commands->start_small_buffer());
		}
		else
		{
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			{
				create_device_buffer(usage, memory_usage);
				void *mapped_data;
				vmaMapMemory(device->get_vma_allocator(), allocations[i], &mapped_data);

				if constexpr (std::is_const_v<T> && std::is_pointer_v<T>)
				{
					memcpy(mapped_data, static_cast<const void *>(data), size);
				}
				else
				{
					memcpy(mapped_data, reinterpret_cast<const void *>(&data), size);
				}

				data_mapped.push_back(mapped_data);
				spdlog::info("buffers: MFIF={}, buffers.size()={}, index={}, T={}", MAX_FRAMES_IN_FLIGHT, buffers.size(), i, typeid(T).name());
			}
		}
	}

	template<class T>
	void buffer<T>::create_staging_buffer(const T &data, size_t size)
	{
		VkBuffer staging_buffer;
		VmaAllocation staging_allocation;

		// Create the staging buffer.
		{
			vk::BufferCreateInfo staging_buffer_info({}, size, vk::BufferUsageFlagBits::eTransferSrc);
			VmaAllocationCreateInfo staging_alloc_info = { 0, VMA_MEMORY_USAGE_CPU_ONLY };

			VkBufferCreateInfo staging_info = static_cast<VkBufferCreateInfo>(staging_buffer_info);

			vmaCreateBuffer(device->get_vma_allocator(),
				&staging_info,
				&staging_alloc_info,
				&staging_buffer,
				&staging_allocation,
				nullptr);

			void *staging_data;
			vmaMapMemory(device->get_vma_allocator(), staging_allocation, &staging_data);

			if constexpr (std::is_const_v<T> || std::is_pointer_v<T>)
			{
				memcpy(staging_data, static_cast<const void *>(data), size);
			}
			else
			{
				memcpy(staging_data, reinterpret_cast<const void *>(&data), size);
			}

			vmaUnmapMemory(device->get_vma_allocator(), staging_allocation);
		}

		buffers.push_back(staging_buffer);
	}

	template<class T>
	void buffer<T>::create_device_buffer(vk::BufferUsageFlags usage, vma::memory_usage memory_usage)
	{
		vk::BufferCreateInfo device_buffer_info({}, size, usage, vk::SharingMode::eExclusive);
		VmaAllocationCreateInfo device_alloc_info = { 0, vma::to_vma_memory_usage(memory_usage), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT };
		VmaAllocation allocation;
		VkBuffer buffer;

		VkBufferCreateInfo device_info = static_cast<VkBufferCreateInfo>(device_buffer_info);

		vmaCreateBuffer(device->get_vma_allocator(),
			&device_info,
			&device_alloc_info,
			&buffer,
			&allocation,
			nullptr);

		buffers.push_back(buffer);
		allocations.push_back(allocation);
	}

	template<class T>
	void buffer<T>::copy_data(vk::CommandBuffer command_buffer)
	{
		commands->begin(command_buffer);
		vk::BufferCopy copy_region(0, 0, size);
		command_buffer.copyBuffer(buffers[0], buffers[1], 1, &copy_region);
		commands->submit_and_wait(command_buffer);

		std::swap(buffers[0], buffers[1]);
	}

	template<class T>
	void buffer<T>::destroy()
	{
		for (int i = 0; i < buffers.size(); i++)
		{
			vmaDestroyBuffer(device->get_vma_allocator(), buffers[i], allocations[i]);
		}
	}

	template<class T>
	buffer<T>::~buffer()
	{
		destroy();
	}
}
