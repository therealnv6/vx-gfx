#include "context.h"
#include <renderpass.h>
#include <vulkan/vulkan_enums.hpp>

namespace gfx
{
    render_pass::render_pass(std::shared_ptr<gfx::context> context)
        : context { context }
    {
        this->create_render_pass();
    }

    render_pass::~render_pass()
    {
        for (auto framebuffer : swap_chain_framebuffers)
        {
            context->device->destroyFramebuffer(framebuffer);
        }

        context->device->destroyRenderPass(this->pass);
    }

    void render_pass::create_render_pass()
    {
        vk::AttachmentDescription color_attachment({},
            context->swap_chain_image_format, // format
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

        this->pass = context->device->createRenderPass(info);
    }

    void render_pass::create_frame_buffers()
    {
        swap_chain_framebuffers.resize(context->swap_chain_images.size());

        for (auto i = 0; i < context->swap_chain_image_views.size(); i++)
        {
            vk::ImageView attachments[] = {
                context->swap_chain_image_views[i]
            };

            vk::FramebufferCreateInfo create_info {
                {},
                this->pass,
                sizeof(attachments) / sizeof(vk::ImageView),
                attachments,
                context->swap_chain_extent.width,
                context->swap_chain_extent.height,
                1
            };

            this->swap_chain_framebuffers[i] = context->device->createFramebuffer(create_info);
        }
    }
}