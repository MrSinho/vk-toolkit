#include "scene.h"



static void destroyRenderingMemory(VkDevice device, RenderingMemory* pMemory) {
	if (device == VK_NULL_HANDLE || pMemory == NULL) {
		return;
	}

	for (uint32_t frame = 0; frame < pMemory->frameCount; frame++) {
		vkDestroyImageView(device, pMemory->depthImages[frame].view, NULL);
		vkDestroyImageView(device, pMemory->colorImages[frame].view, NULL);
		vkDestroyImage(device, pMemory->depthImages[frame].image, NULL);
		vkDestroyImage(device, pMemory->colorImages[frame].image, NULL);
	}
	vkFreeMemory(device, pMemory->memory, NULL);
	*pMemory = (RenderingMemory) {};
}

static uint8_t createRenderingMemory(CreateRenderingResourcesArgs args) {
	RenderingMemory memory = {};
	VkExtent3D imageExtent = {
		.width = args.extent.width,
		.height = args.extent.height,
		.depth = 1
	};

	uint32_t memoryTypeBits = UINT32_MAX;
	for (uint32_t frame = 0; frame < args.frameCount; frame++) {
		VkResult result = createImage2D(
			args.device,
			imageExtent,
			args.colorFormat,
			args.sampleCount,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			&memory.colorImages[frame]
		);
		if (result != VK_SUCCESS) {
			destroyRenderingMemory(args.device, &memory);
			return 0;
		}

		result = createImage2D(
			args.device,
			imageExtent,
			args.depthFormat,
			args.sampleCount,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			&memory.depthImages[frame]
		);
		if (result != VK_SUCCESS) {
			vkDestroyImage(args.device, memory.colorImages[frame].image, NULL);
			destroyRenderingMemory(args.device, &memory);
			return 0;
		}

		memory.frameCount++;
		memoryTypeBits &= memory.colorImages[frame].memoryRequirements.memoryTypeBits;
		memoryTypeBits &= memory.depthImages[frame].memoryRequirements.memoryTypeBits;
	}

	uint32_t memoryTypeIndex = 0;
	VkShellResult shellResult = vkShellGetMemoryTypeIndex(
		args.device,
		args.physicalDevice,
		memoryTypeBits,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&memoryTypeIndex
	);
	if (shellResult != VK_SHELL_SUCCESS) {
		destroyRenderingMemory(args.device, &memory);
		return 0;
	}

	VkDeviceSize allocationSize = 0;
	for (uint32_t frame = 0; frame < args.frameCount; frame++) {
		memory.colorImages[frame].memoryOffset = vkShellGetNextBlockOffset(
			allocationSize,
			memory.colorImages[frame].memoryRequirements.alignment
		);
		allocationSize = memory.colorImages[frame].memoryOffset +
			memory.colorImages[frame].memoryRequirements.size;

		memory.depthImages[frame].memoryOffset = vkShellGetNextBlockOffset(
			allocationSize,
			memory.depthImages[frame].memoryRequirements.alignment
		);
		allocationSize = memory.depthImages[frame].memoryOffset +
			memory.depthImages[frame].memoryRequirements.size;
	}

	VkMemoryAllocateInfo allocateInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = NULL,
		.allocationSize = allocationSize,
		.memoryTypeIndex = memoryTypeIndex
	};

	VkResult result = vkAllocateMemory(args.device, &allocateInfo, NULL, &memory.memory);
	if (result != VK_SUCCESS) {
		destroyRenderingMemory(args.device, &memory);
		return 0;
	}

	for (uint32_t frame = 0; frame < args.frameCount; frame++) {
		result = vkBindImageMemory(
			args.device,
			memory.colorImages[frame].image,
			memory.memory,
			memory.colorImages[frame].memoryOffset
		);
		if (result == VK_SUCCESS) {
			result = vkBindImageMemory(
				args.device,
				memory.depthImages[frame].image,
				memory.memory,
				memory.depthImages[frame].memoryOffset
			);
		}
		if (result != VK_SUCCESS) {
			destroyRenderingMemory(args.device, &memory);
			return 0;
		}
	}

	for (uint32_t frame = 0; frame < args.frameCount; frame++) {
		result = createImageView2D(
			args.device,
			memory.colorImages[frame].image,
			memory.colorImages[frame].format,
			VK_IMAGE_ASPECT_COLOR_BIT,
			&memory.colorImages[frame].view
		);
		if (result == VK_SUCCESS) {
			result = createImageView2D(
				args.device,
				memory.depthImages[frame].image,
				memory.depthImages[frame].format,
				VK_IMAGE_ASPECT_DEPTH_BIT,
				&memory.depthImages[frame].view
			);
		}
		if (result != VK_SUCCESS) {
			destroyRenderingMemory(args.device, &memory);
			return 0;
		}
	}

	destroyRenderingMemory(args.device, &args.pResources->memory);
	args.pResources->memory = memory;
	return 1;
}

uint8_t createRenderingResources(CreateRenderingResourcesArgs args) {
	if (
		args.device == VK_NULL_HANDLE || args.physicalDevice == VK_NULL_HANDLE ||
		args.pSwapchainImages == NULL || args.pSwapchainImageViews == NULL ||
		args.pResources == NULL || args.extent.width == 0 || args.extent.height == 0 ||
		args.colorFormat == VK_FORMAT_UNDEFINED || args.depthFormat == VK_FORMAT_UNDEFINED ||
		args.sampleCount == 0 || args.frameCount == 0 ||
		args.frameCount > SCENE_MAX_FRAMES_IN_FLIGHT ||
		args.swapchainImageCount == 0 || args.swapchainImageCount > args.swapchainImageCapacity ||
		args.swapchainImageCapacity > MAX_STACK_SWAPCHAIN_IMAGE_COUNT
	) {
		return 0;
	}

	if (!createRenderingMemory(args)) {
		return 0;
	}

	for (uint32_t i = 0; i < SCENE_MAX_RENDERING_RESOURCE_COUNT; i++) {
		args.pResources->data[i] = (RenderingData) {};
	}

	for (uint32_t frame = 0; frame < args.frameCount; frame++) {
		for (uint32_t swapchainImage = 0; swapchainImage < args.swapchainImageCount; swapchainImage++) {
			uint32_t index = frame * args.swapchainImageCapacity + swapchainImage;
			RenderingData* pData = &args.pResources->data[index];
			RenderingBarriers* pBarriers = &pData->barriers;

			pData->colorAttachmentInfo = (VkRenderingAttachmentInfo) {
				.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
				.pNext = NULL,
				.imageView = args.pResources->memory.colorImages[frame].view,
				.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT,
				.resolveImageView = args.pSwapchainImageViews[swapchainImage],
				.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE
			};

			pData->depthAttachmentInfo = (VkRenderingAttachmentInfo) {
				.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
				.pNext = NULL,
				.imageView = args.pResources->memory.depthImages[frame].view,
				.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.clearValue = { .depthStencil = { .depth = 1.0f, .stencil = 0 } }
			};

			pData->renderingInfo = (VkRenderingInfo) {
				.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
				.pNext = NULL,
				.flags = 0,
				.renderArea = { .offset = { 0, 0 }, .extent = args.extent },
				.layerCount = 1,
				.viewMask = 0,
				.colorAttachmentCount = 1,
				.pColorAttachments = &pData->colorAttachmentInfo,
				.pDepthAttachment = &pData->depthAttachmentInfo,
				.pStencilAttachment = NULL
			};

			pBarriers->msaaToColorAttachmentMemoryBarrier = (VkImageMemoryBarrier2) {
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
				.image = args.pResources->memory.colorImages[frame].image,
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1
				}
			};

			pBarriers->swapchainToColorAttachmentMemoryBarrier = pBarriers->msaaToColorAttachmentMemoryBarrier;
			pBarriers->swapchainToColorAttachmentMemoryBarrier.image = args.pSwapchainImages[swapchainImage];

			pBarriers->depthToDepthAttachmentMemoryBarrier = (VkImageMemoryBarrier2) {
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
				.pNext = NULL,
				.srcStageMask = VK_PIPELINE_STAGE_2_NONE,
				.srcAccessMask = VK_ACCESS_2_NONE,
				.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
					VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
				.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
					VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
				.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.image = args.pResources->memory.depthImages[frame].image,
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1
				}
			};

			pBarriers->msaaToColorAttachmentDependencyInfo = (VkDependencyInfo) {
				.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
				.pNext = NULL,
				.imageMemoryBarrierCount = 1,
				.pImageMemoryBarriers = &pBarriers->msaaToColorAttachmentMemoryBarrier
			};
			pBarriers->swapchainToColorAttachmentDependencyInfo = (VkDependencyInfo) {
				.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
				.pNext = NULL,
				.imageMemoryBarrierCount = 1,
				.pImageMemoryBarriers = &pBarriers->swapchainToColorAttachmentMemoryBarrier
			};
			pBarriers->depthToDepthAttachmentDependencyInfo = (VkDependencyInfo) {
				.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
				.pNext = NULL,
				.imageMemoryBarrierCount = 1,
				.pImageMemoryBarriers = &pBarriers->depthToDepthAttachmentMemoryBarrier
			};
		}
	}

	VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_STATUS, "Rendering resources initialized");
	return 1;
}

void destroyRenderingResources(VkDevice device, RenderingResources* pResources) {
	if (device == VK_NULL_HANDLE || pResources == NULL) {
		return;
	}

	destroyRenderingMemory(device, &pResources->memory);
	*pResources = (RenderingResources) {};
}
