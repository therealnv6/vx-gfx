#pragma once
#include <cstdint>
#define GLFW_INCLUDE_VULKAN

#include <cstdlib>
#include <functional>
#include <iostream>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp> // modern c++!

#include <limits>
#include <memory>
#include <optional>
#include <ranges>
#include <set>

inline VkResult create_debug_utils_messenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

inline void destroy_debug_utils_messenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

namespace gfx
{
    static const std::vector<const char *> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME,
    };

    static const std::vector<const char *> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };

    typedef vk::PresentModeKHR present_mode;
    typedef vk::SurfaceFormatKHR surface_format;

    typedef std::vector<present_mode> present_modes;
    typedef std::vector<surface_format> surface_formats;

    struct queue_family_indices
    {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        bool is_complete()
        {
            return graphics_family.has_value() && present_family.has_value();
        }
    };

    struct swapchain_support_details
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> present_modes;

        bool is_complete()
        {
            return !this->formats.empty() && !this->present_modes.empty();
        }
    };

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    class context
    {
    public:
        vk::QueueFlagBits flag_bits = vk::QueueFlagBits::eGraphics;

        vk::Queue present_queue;
        vk::Queue graphics_queue;

        // vulkan objects
        vk::SwapchainKHR swap_chain;
        vk::Instance instance;
        vk::SurfaceKHR surface;
        vk::DebugUtilsMessengerEXT debug_messenger;

        vk::CommandPool command_pool;
        vk::CommandBuffer command_buffer;

        vk::Semaphore image_available_semaphore;
        vk::Semaphore render_finished_semaphore;

        vk::Fence in_flight_fence;

        std::vector<vk::Image> swap_chain_images;
        std::vector<vk::ImageView> swap_chain_image_views;

        vk::Format swap_chain_image_format;
        vk::Extent2D swap_chain_extent;

        // devices
        vk::UniqueDevice device;
        std::optional<vk::PhysicalDevice> physical_device;

        GLFWwindow *window;

        std::function<bool(vk::PhysicalDevice)> device_suitable = [](vk::PhysicalDevice device)
        {
            return true;
        };

        std::function<gfx::surface_format(gfx::surface_formats &available_formats)> choose_swap_surface = [](gfx::surface_formats &available_formats)
        {
            for (const auto &available_format : available_formats)
            {
                if (available_format.format == vk::Format::eB8G8R8A8Srgb && available_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                {
                    return available_format;
                }
            }

            return available_formats[0];
        };

        std::function<gfx::present_mode(const gfx::present_modes &available_present_modes)> choose_present_mode = [](const gfx::present_modes &available_present_modes)
        {
            return gfx::present_mode::eImmediate;
        };

        context(uint32_t width, uint32_t height, const char *name);
        ~context();

        static context &current()
        {
            return *context::current_context;
        }

        void setup_debug_messenger()
        {
            if (!enable_validation_layers)
            {
                return;
            }

            typedef vk::DebugUtilsMessageSeverityFlagBitsEXT severity;
            typedef vk::DebugUtilsMessageTypeFlagBitsEXT type;

            vk::DebugUtilsMessengerCreateInfoEXT createInfo({},
                severity::eInfo | severity::eWarning | severity::eError | severity::eWarning, // messageSeverity
                type::eGeneral | type::eValidation | type::ePerformance, // messageType
                gfx::debug_callback // pfnUserCallback
            );

            auto leg_vk = static_cast<VkDebugUtilsMessengerCreateInfoEXT>(createInfo);
            auto oth_leg_vk = static_cast<VkDebugUtilsMessengerEXT>(debug_messenger);

            auto result = create_debug_utils_messenger(this->instance, &leg_vk, nullptr, &oth_leg_vk);

            if (result != VK_SUCCESS)
            {
                throw std::runtime_error("failed to set up debug messenger: " + std::to_string(result));
            }
        }

        std::vector<const char *> get_required_extensions()
        {
            uint32_t glfw_extension_count = 0;
            const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

            std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

            if (enable_validation_layers)
            {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }

            return extensions;
        }

        bool check_validation_support()
        {
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            for (const char *layerName : validation_layers)
            {
                bool layerFound = false;

                for (const auto &layerProperties : availableLayers)
                {
                    if (strcmp(layerName, layerProperties.layerName) == 0)
                    {
                        layerFound = true;
                        break;
                    }
                }

                if (!layerFound)
                {
                    return false;
                }
            }

            return true;
        }

        void draw(
            std::function<void(vk::CommandBuffer, uint32_t)> record_command_buffer,
            std::function<vk::CommandBufferBeginInfo(gfx::context &context)> begin = [](gfx::context &context)
            { return vk::CommandBufferBeginInfo {}; })
        {
            vk::ResultValue<uint32_t> image_index = device->acquireNextImageKHR(swap_chain, UINT64_MAX, image_available_semaphore);
            vk::CommandBufferBeginInfo begin_info = begin(*this);

            command_buffer.begin(begin_info);

            if (device->waitForFences(in_flight_fence, true, UINT64_MAX) != vk::Result::eSuccess)
            {
                std::cerr << "unable to wait for fence in_flight_fence!" << std::endl;
            }

            record_command_buffer(this->command_buffer, image_index.value);

            vk::Semaphore wait_semaphores[] = { image_available_semaphore };
            vk::Semaphore signal_semaphores[] = { render_finished_semaphore };

            vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

            vk::SubmitInfo submit_info {
                sizeof(wait_semaphores) / sizeof(vk::Semaphore),
                wait_semaphores,
                wait_stages,
                1,
                &command_buffer,
                sizeof(signal_semaphores) / sizeof(vk::Semaphore),
                signal_semaphores,
            };

            graphics_queue.submit(submit_info, in_flight_fence);
            device->resetFences(in_flight_fence);

            vk::SwapchainKHR swap_chains[] = { swap_chain };
            vk::PresentInfoKHR present_info {
                sizeof(wait_semaphores) / sizeof(vk::Semaphore),
                signal_semaphores,
                sizeof(swap_chains) / sizeof(vk::SwapchainKHR),
                swap_chains,
            };

            vk::Result result = present_queue.presentKHR(present_info);

            if (result != vk::Result::eSuccess)
            {
                throw std::runtime_error("unable to present info!");
            }
        }

    private:
        const uint32_t width;
        const uint32_t height;
        const bool enable_validation_layers = true;

        std::string window_name;

        static context *current_context;

        void init_window();
        void setup_vulkan_device();
        void create_surface();
        void create_swap_chain();
        void create_image_views();
        void create_frame_buffers();
        void create_sync_objects();
        void create_command_pool();
        void create_command_buffer();

        vk::Extent2D choose_swap_extent(const vk::SurfaceCapabilitiesKHR &capabilities);
    };
}