#include "scene.h"

VkResult recreateSwapchain(
	int width,
	int height,
	VkSampleCountFlagBits sampleCount,
	Driver* pDriver,
	Queues* pQueues,
	SurfaceData* pSurfaceData,
	SwapchainData* pSwapchainData,
	FrameData* pFrameData,
	RenderingResources* pRenderingResources
) {
	uint32_t oldSwapchainImageCount = pSwapchainData->swapchainImageCount;
	VkFormat oldSwapchainImageFormat = pSwapchainData->swapchainImageFormat;
	VkFormat depthFormat = pRenderingResources->memory.depthImages[0].format;

	VkResult result = vkToolkitRecreateSwapchain((RecreateSwapchainArgs) {
		.width = width,
		.height = height,
		.vsync = 0,
		.pDriver = pDriver,
		.pQueues = pQueues,
		.pSurfaceData = pSurfaceData,
		.pSwapchainData = pSwapchainData
	});
	if (result != VK_SUCCESS) {
		return result;
	}

	if (pSwapchainData->swapchainImageFormat != oldSwapchainImageFormat) {
		return VK_ERROR_FORMAT_NOT_SUPPORTED;
	}
	if (pSwapchainData->swapchainImageCount < pFrameData->framesInFlight) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (pSwapchainData->swapchainImageCount != oldSwapchainImageCount) {
		for (uint32_t i = 0; i < oldSwapchainImageCount; i++) {
			vkDestroySemaphore(pDriver->device, pQueues->graphicsQueueFinishedSemaphores[i], NULL);
			pQueues->graphicsQueueFinishedSemaphores[i] = VK_NULL_HANDLE;
		}

		result = vkShellCreateSemaphores(
			pDriver->device,
			pSwapchainData->swapchainImageCount,
			pQueues->graphicsQueueFinishedSemaphores
		);
		if (result != VK_SUCCESS) {
			return result;
		}
	}

	if (createRenderingResources((CreateRenderingResourcesArgs) {
		.device = pDriver->device,
		.physicalDevice = pDriver->physicalDevice,
		.extent = pSwapchainData->extent,
		.colorFormat = pSwapchainData->swapchainImageFormat,
		.depthFormat = depthFormat,
		.sampleCount = sampleCount,
		.frameCount = pFrameData->framesInFlight,
		.swapchainImageCount = pSwapchainData->swapchainImageCount,
		.swapchainImageCapacity = MAX_STACK_SWAPCHAIN_IMAGE_COUNT,
		.pSwapchainImages = pSwapchainData->swapchainImages,
		.pSwapchainImageViews = pSwapchainData->swapchainImageViews,
		.pResources = pRenderingResources
	}) == 0) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	return VK_SUCCESS;
}
