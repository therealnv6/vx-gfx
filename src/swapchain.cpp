#include "global.h"
#include <context.h>
#include <swapchain.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace gfx
{
    swapchain::~swapchain()
    {
        this->cleanup();
    }

    void swapchain::cleanup()
    {
        for (auto image_view : image_views)
        {
            device->logical_device.destroyImageView(image_view);
        }

        device->logical_device.destroySwapchainKHR(chain);
    }

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

    void swapchain::create_image_views()
    {
        this->image_views.resize(this->images.size());

        for (auto i = 0; i < this->images.size(); i++)
        {
            vk::Image image = this->images[i];
            vk::ImageViewType view_type = vk::ImageViewType::e2D;
            vk::Format format = image_format;
            vk::ComponentMapping compmonent_mapping = vk::ComponentMapping(
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity);

            vk::ImageSubresourceRange subsource_range = vk::ImageSubresourceRange(
                vk::ImageAspectFlagBits::eColor,
                0,
                1,
                0,
                1);

            vk::ImageViewCreateInfo create_info({},
                image,
                view_type,
                format,
                compmonent_mapping,
                subsource_range);

            image_views[i] = device->logical_device.createImageView(create_info);
        }
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

    render_pass::render_pass(gfx::swapchain *swapchain, vk::SampleCountFlags samples, vk::AttachmentStoreOp store_operation, vk::AttachmentLoadOp load_operation, vk::AttachmentLoadOp stencil_load_op, vk::AttachmentStoreOp stencil_store_op, vk::ImageLayout initial_layout, vk::ImageLayout final_layout)
        : swapchain(swapchain),
        device {swapchain->device}
    {
        // Set render pass options
        this->samples = samples;
        this->store_operation = store_operation;
        this->load_operation = load_operation;
        this->stencil_load_op = stencil_load_op;
        this->stencil_store_op = stencil_store_op;
        this->initial_layout = initial_layout;
        this->final_layout = final_layout;

        // Create render pass and frame buffers
        this->create_render_pass();
        this->create_frame_buffers();
    }

    render_pass start_render_pass(gfx::swapchain *swapchain, vk::SampleCountFlags samples, vk::AttachmentStoreOp store_operation, vk::AttachmentLoadOp load_operation, vk::AttachmentLoadOp stencil_load_op, vk::AttachmentStoreOp stencil_store_op, vk::ImageLayout initial_layout, vk::ImageLayout final_layout)
    {
        return render_pass(swapchain, samples, store_operation, load_operation, stencil_load_op, stencil_store_op, initial_layout, final_layout);
    }

    void render_pass::cleanup()
    {
        for (auto framebuffer : framebuffers)
        {
            device->logical_device.destroyFramebuffer(framebuffer);
        }

        device->logical_device.destroyRenderPass(pass);
    }

    void render_pass::create_render_pass()
    {
        vk::AttachmentDescription color_attachment({},
            swapchain->image_format, // format
            vk::SampleCountFlagBits::e1, // samples
            vk::AttachmentLoadOp::eClear, // loadOp
            vk::AttachmentStoreOp::eStore, // storeOp
            vk::AttachmentLoadOp::eDontCare, // stencilLoadOp
            vk::AttachmentStoreOp::eDontCare, // stencilStoreOp
            vk::ImageLayout::eUndefined, // initialLayout
            vk::ImageLayout::ePresentSrcKHR // finalLayout
        );

        vk::AttachmentReference color_attachment_ref(0, vk::ImageLayout::eColorAttachmentOptimal);

        vk::SubpassDescription subpass({},
            vk::PipelineBindPoint::eGraphics, // pipelineBindPoint
            color_attachment_ref // pColorAttachments
        );

        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;

        vk::SubpassDependency dependency {
            VK_SUBPASS_EXTERNAL,
            0,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::AccessFlagBits::eNone,
            vk::AccessFlagBits::eColorAttachmentWrite,
        };

        vk::RenderPassCreateInfo info({}, 1, &color_attachment, 1, &subpass, 1, &dependency);

        this->pass = device->logical_device.createRenderPass(info);
    }

    void render_pass::create_frame_buffers()
    {
        framebuffers.resize(swapchain->images.size());

        for (auto i = 0; i < swapchain->image_views.size(); i++)
        {
            vk::ImageView attachments[] = {
                swapchain->image_views[i]
            };

            vk::FramebufferCreateInfo create_info {
                {},
                this->pass,
                sizeof(attachments) / sizeof(vk::ImageView),
                attachments,
                swapchain->extent.width,
                swapchain->extent.height,
                1
            };

            this->framebuffers[i] = device->logical_device.createFramebuffer(create_info);
        }
    }
}