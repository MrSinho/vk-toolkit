#include <math.h>

#include "clear-color.h"



int main(void) {
	GLFWwindow* window = NULL;
	Driver driver = { 0 };
	Queues queues = { 0 };
	Commands commands = { 0 };
	SurfaceData surfaceData = { 0 };
	SwapchainData swapchainData = { 0 };
	FrameData frameData = { 0 };
	RenderingResources renderingResources[MAX_STACK_SWAPCHAIN_IMAGE_COUNT] = { 0 };
	int width = 0;
	int height = 0;
	const uint8_t vsync = 0;

	window = initializeWindow("vulkan clear color", &width, &height);
	if (window == NULL) {
		return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, 1);
	}
	glfwGetFramebufferSize(window, &width, &height);

	VK_TOOLKIT_SHELL_CHECK(
		initializeVulkan(window, VK_API_VERSION_1_4, &driver, &queues, &surfaceData),
		return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, 1)
	);
	VK_TOOLKIT_VK_RESULT_ERROR(
		createSwapchain(width, height, VK_FORMAT_R8G8B8_UNORM, vsync, VK_NULL_HANDLE, &driver, &queues, &surfaceData, &swapchainData),
		return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, 1)
	);

	VK_TOOLKIT_VK_RESULT_ERROR(
		createSwapchainResources(&driver, &queues, &commands, &swapchainData, &frameData),
		return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, 1)
	);

	if (createRenderingResources(
		swapchainData.extent,
		swapchainData.swapchainImageCount,
		MAX_STACK_SWAPCHAIN_IMAGE_COUNT,
		swapchainData.swapchainImages,
		swapchainData.swapchainImageViews,
		renderingResources
	) == 0) {
		return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, 1);
	}

	uint32_t swapchainImageIndex = 0;
	uint8_t recreateSwapchainRequested = 0;
	uint32_t frameIndex = 0;

	VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_STATUS, "Entering main loop");

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		int framebufferWidth = 0;
		int framebufferHeight = 0;
		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

		if (framebufferWidth == 0 || framebufferHeight == 0) {
			glfwWaitEvents();
			continue;
		}

		if (framebufferWidth != width || framebufferHeight != height || recreateSwapchainRequested) {
			width = framebufferWidth;
			height = framebufferHeight;

			VK_TOOLKIT_VK_RESULT_ERROR(
				recreateSwapchain(
					width,
					height,
					vsync,
					&driver,
					&queues,
					&surfaceData,
					&swapchainData,
					&frameData,
					renderingResources
				),
				return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, 1)
			);
			swapchainImageIndex = 0;
			frameIndex = 0;
			recreateSwapchainRequested = 0;
			continue;
		}

		VK_TOOLKIT_VK_RESULT_ERROR(
			vkWaitForFences(driver.device, 1, &commands.graphicsCmdFences[frameIndex], VK_TRUE, UINT64_MAX),
			return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, 1)
		);

		VkResult acquireResult = vkAcquireNextImageKHR(
			driver.device,
			swapchainData.swapchain,
			UINT64_MAX,
			queues.imageAcquiredSemaphores[frameIndex],
			VK_NULL_HANDLE,
			&swapchainImageIndex
		);
		if (acquireResult == VK_SUBOPTIMAL_KHR) {
			recreateSwapchainRequested = 1;
		}
		else if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapchainRequested = 1;
			continue;
		}
		else if (acquireResult != VK_SUCCESS) {
			VK_TOOLKIT_VK_RESULT_ERROR(acquireResult, return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, 1));
		}

		VK_TOOLKIT_VK_RESULT_ERROR(
			vkResetFences(driver.device, 1, &commands.graphicsCmdFences[frameIndex]),
			return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, 1)
		);

		VkCommandBufferBeginInfo beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = NULL,
			.flags = 0,
			.pInheritanceInfo = NULL
		};
		VK_TOOLKIT_VK_RESULT_ERROR(
			vkBeginCommandBuffer(commands.graphicsCmdBuffers[frameIndex], &beginInfo),
			return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, 1)
		);

		double time = glfwGetTime();
		float red = (float)sin(time);
		float green = (float)cos(time);
		float blue = (float)sin(time + 3.141519f / 2.0f) + 0.2f;

		RenderingResources* pRenderingResources = &renderingResources[swapchainImageIndex];
		pRenderingResources->colorAttachmentInfo.clearValue = (VkClearValue) {
			.color = {
				.float32 = { red, green, blue, 1.0f }
			}
		};

		vkCmdPipelineBarrier2(
			commands.graphicsCmdBuffers[frameIndex],
			&pRenderingResources->toColorAttachmentDependencyInfo
		);

		vkCmdBeginRendering(commands.graphicsCmdBuffers[frameIndex], &pRenderingResources->renderingInfo);

		setViewport(commands.graphicsCmdBuffers[frameIndex], swapchainData.extent);

		endRendering(
			commands.graphicsCmdBuffers[frameIndex],
			swapchainData.swapchainImages[swapchainImageIndex]
		);

		VK_TOOLKIT_VK_RESULT_ERROR(
			vkEndCommandBuffer(commands.graphicsCmdBuffers[frameIndex]),
			return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, 1)
		);

		VK_TOOLKIT_VK_RESULT_ERROR(
			submitGraphics(
				queues.graphicsQueue,
				commands.graphicsCmdBuffers[frameIndex],
				commands.graphicsCmdFences[frameIndex],
				queues.imageAcquiredSemaphores[frameIndex],
				queues.graphicsQueueFinishedSemaphores[swapchainImageIndex]
			),
			return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, 1)
		);

		VkResult swapchainResult = VK_SUCCESS;
		VkResult presentResult = presentImage(
			queues.presentQueue,
			queues.graphicsQueueFinishedSemaphores[swapchainImageIndex],
			swapchainData.swapchain,
			swapchainImageIndex,
			&swapchainResult
		);

		if (
			presentResult == VK_SUBOPTIMAL_KHR || presentResult == VK_ERROR_OUT_OF_DATE_KHR ||
			swapchainResult == VK_SUBOPTIMAL_KHR || swapchainResult == VK_ERROR_OUT_OF_DATE_KHR
		) {
			recreateSwapchainRequested = 1;
		}
		else {
			VK_TOOLKIT_VK_RESULT_ERROR(presentResult, return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, 1));
			VK_TOOLKIT_VK_RESULT_ERROR(swapchainResult, return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, 1));
		}

		frameIndex = (frameIndex + 1) % frameData.framesInFlight;
	}

	VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_STATUS, "End main loop");
	VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_STATUS, "Application finished successfully");
	return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, 0);
}
