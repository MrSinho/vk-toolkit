#ifndef VK_TOOLKIT_COMMANDS_H
#define VK_TOOLKIT_COMMANDS_H

#include <vulkan/vulkan.h>



static VkResult vkToolkitBeginSingleUseCommandBuffer(
	VkDevice device,
	VkCommandPool commandPool,
	VkCommandBuffer* pCommandBuffer
) {
	if (device == VK_NULL_HANDLE || commandPool == VK_NULL_HANDLE || pCommandBuffer == NULL) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkCommandBufferAllocateInfo allocateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};

	VkResult result = vkAllocateCommandBuffers(device, &allocateInfo, pCommandBuffer);
	if (result != VK_SUCCESS) {
		return result;
	}

	VkCommandBufferBeginInfo beginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		.pInheritanceInfo = NULL
	};

	result = vkBeginCommandBuffer(*pCommandBuffer, &beginInfo);
	if (result != VK_SUCCESS) {
		vkFreeCommandBuffers(device, commandPool, 1, pCommandBuffer);
		*pCommandBuffer = VK_NULL_HANDLE;
	}

	return result;
}

static VkResult vkToolkitEndSubmitSingleUseCommandBuffer(
	VkDevice device,
	VkCommandPool commandPool,
	VkQueue queue,
	VkCommandBuffer commandBuffer
) {
	if (
		device == VK_NULL_HANDLE || commandPool == VK_NULL_HANDLE ||
		queue == VK_NULL_HANDLE || commandBuffer == VK_NULL_HANDLE
	) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkResult result = vkEndCommandBuffer(commandBuffer);
	if (result == VK_SUCCESS) {
		VkCommandBufferSubmitInfo commandBufferInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
			.pNext = NULL,
			.commandBuffer = commandBuffer,
			.deviceMask = 1
		};

		VkSubmitInfo2 submitInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
			.pNext = NULL,
			.flags = 0,
			.commandBufferInfoCount = 1,
			.pCommandBufferInfos = &commandBufferInfo
		};

		result = vkQueueSubmit2(queue, 1, &submitInfo, VK_NULL_HANDLE);
	}

	if (result == VK_SUCCESS) {
		result = vkQueueWaitIdle(queue);
	}

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	return result;
}

#endif//VK_TOOLKIT_COMMANDS_H
