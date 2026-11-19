#include "compute.h"

#include "../common/commands.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

int main(void) {
	Driver driver = {};
	Commands commands = {};
	Memory memory = {};
	Pipeline pipeline = {};
	Descriptors descriptors = {};
	ApplicationResources applicationResources = {
		.pDriver = &driver,
		.pCommands = &commands,
		.pMemory = &memory,
		.pPipeline = &pipeline,
		.pDescriptors = &descriptors
	};

	float values[COMPUTE_VALUE_COUNT] = {};
	const float factor = 1.0f;
	char logMessage[128] = {};

	for (uint32_t i = 0; i < COMPUTE_VALUE_COUNT; i++) {
		values[i] = (float)i;
		snprintf(logMessage, sizeof(logMessage), "input[%u]: %f", i, values[i]);
		VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_DEBUG, logMessage);
	}

	if (initializeVulkan(&driver, &commands) == 0) {
		return cleanupApplication(applicationResources, 1);
	}

	VK_TOOLKIT_VK_RESULT_ERROR(
		createMemory((CreateMemoryArgs) {
			.device = driver.device,
			.physicalDevice = driver.physicalDevice,
			.pValues = values,
			.valueCount = COMPUTE_VALUE_COUNT,
			.factor = factor,
			.pMemory = &memory
		}),
		return cleanupApplication(applicationResources, 1)
	);

	VkCommandBuffer transferCommandBuffer = VK_NULL_HANDLE;
	VK_TOOLKIT_VK_RESULT_ERROR(
		vkToolkitBeginSingleUseCommandBuffer(
			driver.device,
			commands.commandPool,
			&transferCommandBuffer
		),
		return cleanupApplication(applicationResources, 1)
	);

	copyBuffers(transferCommandBuffer, &memory);
	applyBarriers(transferCommandBuffer, &memory);

	VK_TOOLKIT_VK_RESULT_ERROR(
		vkToolkitEndSubmitSingleUseCommandBuffer(
			driver.device,
			commands.commandPool,
			commands.queue,
			transferCommandBuffer
		),
		return cleanupApplication(applicationResources, 1)
	);

	if (createPipeline(driver.device, &pipeline) == 0) {
		return cleanupApplication(applicationResources, 1);
	}

	if (createDescriptors((CreateDescriptorsArgs) {
		.device = driver.device,
		.pMemory = &memory,
		.pPipeline = &pipeline,
		.pDescriptors = &descriptors
	}) == 0) {
		return cleanupApplication(applicationResources, 1);
	}

	VkCommandBufferBeginInfo beginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		.pInheritanceInfo = NULL
	};
	VK_TOOLKIT_VK_RESULT_ERROR(
		vkBeginCommandBuffer(commands.commandBuffer, &beginInfo),
		return cleanupApplication(applicationResources, 1)
	);

	vkCmdBindPipeline(commands.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.pipeline);

	vkCmdBindDescriptorSets(
		commands.commandBuffer,
		VK_PIPELINE_BIND_POINT_COMPUTE,
		pipeline.layout,
		0,
		(uint32_t)descriptors.sets.size(),
		descriptors.sets.data(),
		0,
		NULL
	);

	VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_STATUS, "Squaring the numbers");
	vkCmdDispatch(commands.commandBuffer, COMPUTE_WORKGROUP_COUNT, 1, 1);

	vkCmdPipelineBarrier2(commands.commandBuffer, &memory.barriers.computeToTransferDependencyInfo);

	vkCmdCopyBuffer(
		commands.commandBuffer,
		memory.deviceBuffers[COMPUTE_DEVICE_BUFFER_VALUES].buffer,
		memory.stagingBuffer.buffer,
		1,
		&memory.deviceLocalToStagingCopy
	);

	vkCmdPipelineBarrier2(commands.commandBuffer, &memory.barriers.transferToHostDependencyInfo);

	VK_TOOLKIT_VK_RESULT_ERROR(
		vkEndCommandBuffer(commands.commandBuffer),
		return cleanupApplication(applicationResources, 1)
	);

	VkCommandBufferSubmitInfo commandBufferInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
		.pNext = NULL,
		.commandBuffer = commands.commandBuffer,
		.deviceMask = 1
	};
	VkSubmitInfo2 submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
		.pNext = NULL,
		.flags = 0,
		.commandBufferInfoCount = 1,
		.pCommandBufferInfos = &commandBufferInfo
	};

	VK_TOOLKIT_VK_RESULT_ERROR(
		vkQueueSubmit2(commands.queue, 1, &submitInfo, commands.fence),
		return cleanupApplication(applicationResources, 1)
	);
	VK_TOOLKIT_VK_RESULT_ERROR(
		vkWaitForFences(driver.device, 1, &commands.fence, VK_TRUE, UINT64_MAX),
		return cleanupApplication(applicationResources, 1)
	);

	float outputs[COMPUTE_VALUE_COUNT] = {};
	memcpy(outputs, &memory.pMappedStaging[memory.stagingLayout.valuesOffset], sizeof(outputs));

	VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_STATUS, "Compute shader output values");

	for (uint32_t i = 0; i < COMPUTE_VALUE_COUNT; i++) {
		snprintf(logMessage, sizeof(logMessage), "output[%u]: %f", i, outputs[i]);
		VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_DEBUG, logMessage);

		const float expected = values[i] * values[i] * factor;
		if (fabsf(outputs[i] - expected) > 0.0001f) {
			VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_ERROR, "Unexpected compute output");
			return cleanupApplication(applicationResources, 1);
		}
	}

	VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_STATUS, "Application finished successfully");
	return cleanupApplication(applicationResources, 0);
}
