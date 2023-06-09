#pragma once
#include <swapchain/swapchain.h>

namespace gfx
{
	/**
	 * [commands] class is responsible for managing the command pool, command buffers, and synchronization objects.
	 * It is intended to be used for rendering and synchronization purposes.
	 *
	 * [command_pool] is the pool of command buffers managed by this class.
	 * [command_buffers] is a vector of command buffers allocated from the command pool.
	 * [image_available_semaphores] and [render_finished_semaphores] are semaphores used for synchronization of rendering operations.
	 * [in_flight_fences] are fences used for synchronization of CPU and GPU operations.
	 *
	 * The [commands] constructor takes a [gfx::device] object as input, and creates a command pool with a single command buffer.
	 *
	 * It is recommended to use one set of image available and render finished semaphores, and multiple fences for managing
	 * synchronization between CPU and GPU operations.
	 *
	 * Please note that the [commands] class is not intended to be used for managing swap chains or window management.
	 * For managing swap chains, consider using the [gfx::swapchain] class.
	 */
	class commands
	{
	public:
		// The Vulkan command pool managed by this object.
		vk::CommandPool command_pool;

		// A vector of command buffers allocated from the command pool.
		std::vector<vk::CommandBuffer> command_buffers;

		// A vector of semaphores used for synchronization of rendering operations.
		// Each element of the vector corresponds to an image available semaphore for a swapchain image.
		std::vector<vk::Semaphore> image_available_semaphores;

		// A vector of semaphores used for synchronization of rendering operations.
		// Each element of the vector corresponds to a render finished semaphore for a swapchain image.
		std::vector<vk::Semaphore> render_finished_semaphores;

		// A vector of fences used for synchronization of CPU and GPU operations.
		// Each element of the vector corresponds to a fence for a swapchain image.
		std::vector<vk::Fence> in_flight_fences;
		uint32_t current_frame = 0;

		/**
		 * Constructs a [commands] object using the specified [gfx::device].
		 * A command pool with a single command buffer is created during construction.
		 *
		 * @param swaphcain A pointer to the [gfx::swapchain] object to be used for Vulkan API calls.
		 */
		commands(std::shared_ptr<gfx::swapchain> swapchain, vk::SurfaceKHR *surface);
		~commands();

		void cleanup();

		void create_command_pool();
		void initialize_command_buffers();
		void create_sync_objects();

		void submit_and_wait(const vk::CommandBuffer &command_buffer);
		void begin(const vk::CommandBuffer &command_buffer);
		void submit_nowait(std::function<void(vk::CommandBuffer &buffer)>);

		vk::CommandBuffer start_small_buffer();

	private:
		std::shared_ptr<gfx::swapchain> swapchain;
		std::shared_ptr<gfx::device> device;
		vk::SurfaceKHR *surface;
	};
}
