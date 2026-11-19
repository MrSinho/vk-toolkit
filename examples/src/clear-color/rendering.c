#include "clear-color.h"

uint8_t createRenderingResources(
    VkExtent2D extent,
    uint32_t swapchainImageCount,
    uint32_t resourceCapacity,
    const VkImage* pSwapchainImages,
    const VkImageView* pSwapchainImageViews,
    RenderingResources* pResources
) {
    if (
        extent.width == 0 || extent.height == 0 || swapchainImageCount == 0 ||
        swapchainImageCount > resourceCapacity || pSwapchainImages == NULL ||
        pSwapchainImageViews == NULL || pResources == NULL
    ) {
        return 0;
    }

    for (uint32_t i = 0; i < swapchainImageCount; i++) {
        RenderingResources* pResource = &pResources[i];
        *pResource = (RenderingResources) { 0 };

        pResource->colorAttachmentInfo = (VkRenderingAttachmentInfo) {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .pNext = NULL,
            .imageView = pSwapchainImageViews[i],
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = VK_NULL_HANDLE,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE
        };

        pResource->renderingInfo = (VkRenderingInfo) {
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderArea = {
                .offset = { 0, 0 },
                .extent = extent
            },
            .layerCount = 1,
            .viewMask = 0,
            .colorAttachmentCount = 1,
            .pColorAttachments = &pResource->colorAttachmentInfo,
            .pDepthAttachment = NULL,
            .pStencilAttachment = NULL
        };

        pResource->toColorAttachmentMemoryBarrier = (VkImageMemoryBarrier2) {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext = NULL,
            .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
            .srcAccessMask = VK_ACCESS_2_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = pSwapchainImages[i],
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        pResource->toColorAttachmentDependencyInfo = (VkDependencyInfo) {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .pNext = NULL,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &pResource->toColorAttachmentMemoryBarrier
        };
    }

    return 1;
}
