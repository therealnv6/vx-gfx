#include <renderpass.h>

namespace gfx
{
    void render_pass::create_render_pass(gfx::context &context)
    {
        vk::AttachmentDescription color_attachment({},
            context.swap_chain_image_format, // format
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

        this->render_pass = context.device->createRenderPass(
            vk::RenderPassCreateInfo({}, 1, &color_attachment, 1, &subpass));
    }
}