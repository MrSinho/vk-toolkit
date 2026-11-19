#ifndef VK_TOOLKIT_MEMORY_H
#define VK_TOOLKIT_MEMORY_H

#include "common.h"



typedef struct VkToolkitAllocateBufferMemoryArgs {
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	uint32_t bufferCount;
	VkShellBufferData* pBuffers;
	VkMemoryPropertyFlags memoryProperties;
	uint8_t map;
	VkShellBufferMemoryData* pMemory;
} VkToolkitAllocateBufferMemoryArgs;

static VkResult vkToolkitAllocateBufferMemory(VkToolkitAllocateBufferMemoryArgs args) {
	if (
		args.device == VK_NULL_HANDLE || args.physicalDevice == VK_NULL_HANDLE ||
		args.bufferCount == 0 || args.pBuffers == NULL || args.pMemory == NULL
	) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkShellResult shellResult = vkShellGetBufferMemoryLayout((VkShellGetBufferMemoryLayoutArgs) {
		.bufferCount = args.bufferCount,
		.pBuffers = args.pBuffers,
		.pMemory = args.pMemory
	});
	if (shellResult != VK_SHELL_SUCCESS) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	shellResult = vkShellGetMemoryTypeIndex(
		args.device,
		args.physicalDevice,
		args.pMemory->memoryTypeBits,
		args.memoryProperties,
		&args.pMemory->memoryTypeIndex
	);
	if (shellResult != VK_SHELL_SUCCESS) {
		return VK_ERROR_FEATURE_NOT_PRESENT;
	}

	VkMemoryAllocateInfo allocateInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = NULL,
		.allocationSize = args.pMemory->allocationSize,
		.memoryTypeIndex = args.pMemory->memoryTypeIndex
	};

	VkResult result = vkAllocateMemory(args.device, &allocateInfo, NULL, &args.pMemory->memory);
	if (result != VK_SUCCESS) {
		return result;
	}

	for (uint32_t i = 0; i < args.bufferCount; i++) {
		result = vkBindBufferMemory(
			args.device,
			args.pBuffers[i].buffer,
			args.pMemory->memory,
			args.pBuffers[i].memoryOffset
		);
		if (result != VK_SUCCESS) {
			return result;
		}
	}

	if (args.map) {
		result = vkMapMemory(
			args.device,
			args.pMemory->memory,
			0,
			args.pMemory->allocationSize,
			0,
			&args.pMemory->pMapped
		);
	}

	return result;
}

static VkBufferMemoryBarrier2 vkToolkitCreateBufferMemoryBarrier(
	VkBuffer buffer,
	VkPipelineStageFlags2 srcStageMask,
	VkAccessFlags2 srcAccessMask,
	VkPipelineStageFlags2 dstStageMask,
	VkAccessFlags2 dstAccessMask
) {
	return (VkBufferMemoryBarrier2) {
		.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
		.pNext = NULL,
		.srcStageMask = srcStageMask,
		.srcAccessMask = srcAccessMask,
		.dstStageMask = dstStageMask,
		.dstAccessMask = dstAccessMask,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.buffer = buffer,
		.offset = 0,
		.size = VK_WHOLE_SIZE
	};
}

#endif//VK_TOOLKIT_MEMORY_H
