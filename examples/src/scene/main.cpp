#include "scene.h"

#include "../common/commands.h"

#include <math.h>
#include <string.h>

int main(void) {
	GLFWwindow* window = NULL;
	Driver driver = {};
	Queues queues = {};
	Commands commands = {};
	SurfaceData surfaceData = {};
	SwapchainData swapchainData = {};
	FrameData frameData = {};
	Memory memory = {};
	RenderingResources renderingResources = {};
	Pipeline pipeline = {};
	Descriptors descriptors = {};

	int width  = 0;
	int height = 0;
	window = initializeWindow("vulkan scene", &width, &height);
	if (window == NULL) {
		return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1);
	}
	glfwGetFramebufferSize(window, &width, &height);

	uint32_t sampleCount = 0;

	VK_TOOLKIT_VK_RESULT_ERROR(
		initializeVulkan(window, VK_API_VERSION_1_4, &driver, &queues, &surfaceData),
		return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1)
	);
	VK_TOOLKIT_VK_RESULT_ERROR(
		createSwapchain(width, height, VK_FORMAT_R32G32B32_SFLOAT, 0, VK_NULL_HANDLE, &driver, &queues, &surfaceData, &swapchainData),
		return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1)
	);

	VK_TOOLKIT_VK_RESULT_ERROR(
		createSwapchainResources(&driver, &queues, &commands, &swapchainData, &frameData),
		return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1)
	);

	VK_TOOLKIT_SHELL_CHECK(
		vkShellCombineMaxSamples(driver.physicalDeviceProperties, 8, 1, 1, &sampleCount),
		return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1)
	);

	if (createMemory((CreateMemoryArgs) {
		.device = driver.device,
		.physicalDevice = driver.physicalDevice,
		.frameCount = frameData.framesInFlight,
		.pMemory = &memory
	}) == 0) {
		return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1);
	}

	VkCommandBuffer transferCommandBuffer = VK_NULL_HANDLE;
	VK_TOOLKIT_VK_RESULT_ERROR(
		vkToolkitBeginSingleUseCommandBuffer(
			driver.device,
			commands.graphicsCmdPool,
			&transferCommandBuffer
		),
		return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1)
	);

	copyBuffers(transferCommandBuffer, &memory);
	applyBarriers(transferCommandBuffer, &memory);

	VK_TOOLKIT_VK_RESULT_ERROR(
		vkToolkitEndSubmitSingleUseCommandBuffer(
			driver.device,
			commands.graphicsCmdPool,
			queues.graphicsQueue,
			transferCommandBuffer
		),
		return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1)
	);

	if (createRenderingResources((CreateRenderingResourcesArgs) {
		.device = driver.device,
		.physicalDevice = driver.physicalDevice,
		.extent = swapchainData.extent,
		.colorFormat = swapchainData.swapchainImageFormat,
		.depthFormat = VK_FORMAT_D32_SFLOAT,
		.sampleCount = (VkSampleCountFlagBits)sampleCount,
		.frameCount = frameData.framesInFlight,
		.swapchainImageCount = swapchainData.swapchainImageCount,
		.swapchainImageCapacity = MAX_STACK_SWAPCHAIN_IMAGE_COUNT,
		.pSwapchainImages = swapchainData.swapchainImages,
		.pSwapchainImageViews = swapchainData.swapchainImageViews,
		.pResources = &renderingResources
	}) == 0) {
		return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1);
	}

	if (createPipeline((CreatePipelineArgs) {
		.device = driver.device,
		.sampleCount = (VkSampleCountFlagBits)sampleCount,
		.colorFormat = renderingResources.memory.colorImages[0].format,
		.depthFormat = renderingResources.memory.depthImages[0].format,
		.pPipeline = &pipeline
	}) == 0) {
		return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1);
	}

	if (createDescriptors((CreateDescriptorsArgs) {
		.device = driver.device,
		.frameCount = frameData.framesInFlight,
		.lightBuffer = memory.deviceLocalBuffers[SCENE_DEVICE_BUFFER_UNIFORM].buffer,
		.lightBufferSize = sizeof(light),
		.pPipeline = &pipeline,
		.pDescriptors = &descriptors
	}) == 0) {
		return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1);
	}

	uint32_t swapchainImageIdx = 0;
	uint8_t recreateSwapchainRequested = 0;
	uint32_t frameIndex = 0;

	VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_STATUS, "Entering main loop");

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		int glfwWidth = 0;
		int glfwHeight = 0;
		glfwGetFramebufferSize(window, &glfwWidth, &glfwHeight);

		if (glfwWidth == 0 || glfwHeight == 0) {
			glfwWaitEvents();
			continue;
		}

		if (glfwWidth != width || glfwHeight != height || recreateSwapchainRequested) {
			width = glfwWidth;
			height = glfwHeight;

			VK_TOOLKIT_VK_RESULT_ERROR(
				recreateSwapchain(
					width,
					height,
					(VkSampleCountFlagBits)sampleCount,
					&driver,
					&queues,
					&surfaceData,
					&swapchainData,
					&frameData,
					&renderingResources
				),
				return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1)
			);
			swapchainImageIdx = 0;
			frameIndex = 0;
			recreateSwapchainRequested = 0;
			continue;
		}

		VK_TOOLKIT_VK_RESULT_ERROR(
			vkWaitForFences(driver.device, 1, &commands.graphicsCmdFences[frameIndex], 1, UINT64_MAX),
			return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1)
		);

		VkResult acquireResult = vkAcquireNextImageKHR(
			driver.device,
			swapchainData.swapchain,
			UINT64_MAX,
			queues.imageAcquiredSemaphores[frameIndex],
			VK_NULL_HANDLE,
			&swapchainImageIdx
		);

		if (acquireResult == VK_SUBOPTIMAL_KHR) {
			recreateSwapchainRequested = 1;
		}
		else if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapchainRequested = 1;
			continue;
		}
		else if (acquireResult != VK_SUCCESS) {
			VK_TOOLKIT_VK_RESULT_ERROR(acquireResult, return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1));
		}

		VK_TOOLKIT_VK_RESULT_ERROR(vkResetFences(driver.device, 1, &commands.graphicsCmdFences[frameIndex]), return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1));

		VkCommandBufferBeginInfo commandBufferBeginInfo = {
			.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext            = VK_NULL_HANDLE,
			.flags            = 0,
			.pInheritanceInfo = VK_NULL_HANDLE
		};

		VK_TOOLKIT_VK_RESULT_ERROR(vkBeginCommandBuffer(commands.graphicsCmdBuffers[frameIndex], &commandBufferBeginInfo), return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1));

		float r = 0.1 + 0.01 * (float)sin(glfwGetTime());
		float g = r;
		float b = r;

		uint32_t renderingResourceIndex = frameIndex * MAX_STACK_SWAPCHAIN_IMAGE_COUNT + swapchainImageIdx;
		RenderingData* pRenderingData = &renderingResources.data[renderingResourceIndex];
		pRenderingData->colorAttachmentInfo.clearValue = (VkClearValue) {
			.color = {
				.float32 = { r, g, b, 1.0f }
			}
		};

		vkCmdPipelineBarrier2(
			commands.graphicsCmdBuffers[frameIndex],
			&pRenderingData->barriers.msaaToColorAttachmentDependencyInfo
		);

		vkCmdPipelineBarrier2(
			commands.graphicsCmdBuffers[frameIndex],
			&pRenderingData->barriers.swapchainToColorAttachmentDependencyInfo
		);

		vkCmdPipelineBarrier2(
			commands.graphicsCmdBuffers[frameIndex],
			&pRenderingData->barriers.depthToDepthAttachmentDependencyInfo
		);

		triangle[1].position[1] = (float)sin(glfwGetTime());
		VkDeviceSize triangleStagingOffset = memory.stagingLayout.triangleVerticesOffset + frameIndex * sizeof(triangle);
		memcpy(&memory.pMappedStaging[triangleStagingOffset], triangle, sizeof(triangle));

		vkCmdCopyBuffer(
			commands.graphicsCmdBuffers[frameIndex],
			memory.stagingBuffer.buffer,
			memory.deviceLocalBuffers[SCENE_DEVICE_BUFFER_VERTEX].buffer,
			1,
			&memory.triangleBufferCopies[frameIndex]
		);

		vkCmdPipelineBarrier2(
			commands.graphicsCmdBuffers[frameIndex],
			&memory.barriers.transferToVertexDependencyInfo
		);

		vkCmdBeginRendering(commands.graphicsCmdBuffers[frameIndex], &pRenderingData->renderingInfo);

		VkDeviceSize vertexOffsets[2] = { 0, 0 };
		VkBuffer     vertexBuffers[2] = { memory.deviceLocalBuffers[SCENE_DEVICE_BUFFER_VERTEX].buffer, memory.deviceLocalBuffers[SCENE_DEVICE_BUFFER_INSTANCE].buffer };

		vkCmdBindVertexBuffers(commands.graphicsCmdBuffers[frameIndex], 0, 2, vertexBuffers, vertexOffsets);

		vkCmdBindIndexBuffer(commands.graphicsCmdBuffers[frameIndex], memory.deviceLocalBuffers[SCENE_DEVICE_BUFFER_INDEX].buffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdBindPipeline(commands.graphicsCmdBuffers[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);

		vkCmdPushConstants(commands.graphicsCmdBuffers[frameIndex], pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(projectionView), projectionView);

		uint32_t writeSetIdx = descriptors.lightBinding.set + frameIndex * descriptors.frameSetStride;
		vkCmdBindDescriptorSets(
			commands.graphicsCmdBuffers[frameIndex],
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipeline.layout,
			descriptors.lightBinding.set,
			1,
			&descriptors.sets[writeSetIdx],
			0,
			NULL
		);

		setViewport(commands.graphicsCmdBuffers[frameIndex], swapchainData.extent);

		vkCmdDrawIndexed(commands.graphicsCmdBuffers[frameIndex], QUAD_INDEX_COUNT, 2, 0, 0, 0);

		uint32_t triangleFirstVertex = QUAD_VERTEX_COUNT + frameIndex * TRIANGLE_VERTEX_COUNT;
		vkCmdDraw(commands.graphicsCmdBuffers[frameIndex], TRIANGLE_VERTEX_COUNT, 1, triangleFirstVertex, 2);

		endRendering(
			commands.graphicsCmdBuffers[frameIndex],
			swapchainData.swapchainImages[swapchainImageIdx]
		);

		VK_TOOLKIT_VK_RESULT_ERROR(vkEndCommandBuffer(commands.graphicsCmdBuffers[frameIndex]), return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1));

		VK_TOOLKIT_VK_RESULT_ERROR(
			submitGraphics(
				queues.graphicsQueue,
				commands.graphicsCmdBuffers[frameIndex],
				commands.graphicsCmdFences[frameIndex],
				queues.imageAcquiredSemaphores[frameIndex],
				queues.graphicsQueueFinishedSemaphores[swapchainImageIdx]
			),
			return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1)
		);

		VkResult swapchainResult = VK_SUCCESS;

		VkResult presentResult = presentImage(
				queues.presentQueue,
				queues.graphicsQueueFinishedSemaphores[swapchainImageIdx],
				swapchainData.swapchain,
				swapchainImageIdx,
				&swapchainResult
		);

		if (
			presentResult == VK_SUBOPTIMAL_KHR || presentResult == VK_ERROR_OUT_OF_DATE_KHR ||
			swapchainResult == VK_SUBOPTIMAL_KHR || swapchainResult == VK_ERROR_OUT_OF_DATE_KHR
		) {
			recreateSwapchainRequested = 1;
		}
		else {
			VK_TOOLKIT_VK_RESULT_ERROR(presentResult, return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1));
			VK_TOOLKIT_VK_RESULT_ERROR(swapchainResult, return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 1));
		}

		frameIndex = (frameIndex + 1) % frameData.framesInFlight;
	}

	VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_STATUS, "End mainloop");
	VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_STATUS, "Application finished successfully");
	return cleanup(window, &driver, &queues, &commands, &surfaceData, &swapchainData, &memory, &renderingResources, &pipeline, &descriptors, 0);
}
