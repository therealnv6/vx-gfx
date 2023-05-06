#include "global.h"
#include <context.h>
#include <swapchain.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace gfx
{
    gfx::swapchain_support_details query_swapchain_support(std::optional<vk::PhysicalDevice> device, vk::SurfaceKHR surface)
    {
        return gfx::swapchain_support_details {
            .capabilities = device->getSurfaceCapabilitiesKHR(surface),
            .formats = device->getSurfaceFormatsKHR(surface),
            .present_modes = device->getSurfacePresentModesKHR(surface),
        };
    }

    void swapchain::initialize(GLFWwindow *window, vk::SurfaceKHR &surface)
    {
        gfx::swapchain_support_details details = gfx::query_swapchain_support(device->physical_device, surface);

        this->extent = this->choose_swap_extent(details.capabilities, window);
        this->create_swapchain(surface, details);
    }

    void swapchain::create_swapchain(vk::SurfaceKHR &surface, gfx::swapchain_support_details details)
    {
        auto surface_format = this->choose_swap_surface(details.formats);
        auto present_mode = this->choose_present_mode(details.present_modes);

        this->image_format = surface_format.format;

        auto image_usage = vk::ImageUsageFlagBits::eColorAttachment
            | vk::ImageUsageFlagBits::eInputAttachment;

        uint32_t image_count = std::max(details.capabilities.minImageCount + 1, details.capabilities.maxImageCount);

        vk::SwapchainCreateInfoKHR create_info({}, surface, image_count, surface_format.format, surface_format.colorSpace, extent, 1, image_usage);
        gfx::queue_family_indices indices = device->find_queue_families(&surface, device->physical_device);

        uint32_t queue_family_indices[] = {
            indices.graphics_family.value(),
            indices.present_family.value()
        };

        if (indices.graphics_family != indices.present_family)
        {
            create_info.setImageSharingMode(vk::SharingMode::eConcurrent);
            create_info.setQueueFamilyIndices(queue_family_indices);
        }
        else
        {
            create_info.setImageSharingMode(vk::SharingMode::eExclusive);
        }

        create_info.setImageExtent(extent);
        create_info.setPreTransform(details.capabilities.currentTransform);
        create_info.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        create_info.setPresentMode(present_mode);
        create_info.setClipped(true);

        this->chain = device->logical_device.createSwapchainKHR(create_info);
        this->images = device->logical_device.getSwapchainImagesKHR(chain);
    }

    vk::Extent2D swapchain::choose_swap_extent(const vk::SurfaceCapabilitiesKHR &capabilities, GLFWwindow *window)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width;
            int height;
            glfwGetFramebufferSize(window, &width, &height);

            vk::Extent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }
}