#include "compute.h"

#include "../common/memory.h"

#include <string.h>

void destroyMemory(VkDevice device, Memory* pMemory) {
	if (device == VK_NULL_HANDLE || pMemory == NULL) {
		return;
	}

	if (pMemory->stagingMemory.pMapped != NULL) {
		vkUnmapMemory(device, pMemory->stagingMemory.memory);
	}

	vkDestroyBuffer(device, pMemory->stagingBuffer.buffer, NULL);

	for (uint32_t i = 0; i < COMPUTE_DEVICE_BUFFER_COUNT; i++) {
		vkDestroyBuffer(device, pMemory->deviceBuffers[i].buffer, NULL);
	}

	vkFreeMemory(device, pMemory->stagingMemory.memory, NULL);
	vkFreeMemory(device, pMemory->deviceMemory.memory, NULL);

	*pMemory = (Memory) {};
}

VkResult createMemory(CreateMemoryArgs args) {
	if (
		args.device == VK_NULL_HANDLE || args.physicalDevice == VK_NULL_HANDLE ||
		args.pValues == NULL || args.valueCount == 0 || args.pMemory == NULL
	) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	Memory* pMemory = args.pMemory;
	*pMemory = (Memory) {};

	const VkDeviceSize valuesSize = args.valueCount * sizeof(*args.pValues);
	pMemory->stagingLayout = (StagingLayout) {
		.valuesOffset = 0,
		.factorOffset = valuesSize,
		.size = valuesSize + sizeof(args.factor)
	};

	VkResult result = createBuffer(
		args.device,
		pMemory->stagingLayout.size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		&pMemory->stagingBuffer
	);
	if (result != VK_SUCCESS) {
		destroyMemory(args.device, pMemory);
		return result;
	}

	result = vkToolkitAllocateBufferMemory((VkToolkitAllocateBufferMemoryArgs) {
		.device = args.device,
		.physicalDevice = args.physicalDevice,
		.bufferCount = 1,
		.pBuffers = &pMemory->stagingBuffer,
		.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		.map = 1,
		.pMemory = &pMemory->stagingMemory
	});
	if (result != VK_SUCCESS) {
		destroyMemory(args.device, pMemory);
		return result;
	}
	pMemory->pMappedStaging = (uint8_t*)pMemory->stagingMemory.pMapped;

	memcpy(&pMemory->pMappedStaging[pMemory->stagingLayout.valuesOffset], args.pValues, valuesSize);
	memcpy(&pMemory->pMappedStaging[pMemory->stagingLayout.factorOffset], &args.factor, sizeof(args.factor));

	result = createBuffer(
		args.device,
		valuesSize,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		&pMemory->deviceBuffers[COMPUTE_DEVICE_BUFFER_VALUES]
	);
	if (result != VK_SUCCESS) {
		destroyMemory(args.device, pMemory);
		return result;
	}

	result = createBuffer(
		args.device,
		sizeof(args.factor),
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		&pMemory->deviceBuffers[COMPUTE_DEVICE_BUFFER_FACTOR]
	);
	if (result != VK_SUCCESS) {
		destroyMemory(args.device, pMemory);
		return result;
	}

	result = vkToolkitAllocateBufferMemory((VkToolkitAllocateBufferMemoryArgs) {
		.device = args.device,
		.physicalDevice = args.physicalDevice,
		.bufferCount = COMPUTE_DEVICE_BUFFER_COUNT,
		.pBuffers = pMemory->deviceBuffers,
		.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		.map = 0,
		.pMemory = &pMemory->deviceMemory
	});
	if (result != VK_SUCCESS) {
		destroyMemory(args.device, pMemory);
		return result;
	}

	pMemory->stagingToDeviceLocalCopies[COMPUTE_DEVICE_BUFFER_VALUES] = (VkBufferCopy) {
		.srcOffset = pMemory->stagingLayout.valuesOffset,
		.dstOffset = 0,
		.size = valuesSize
	};

	pMemory->stagingToDeviceLocalCopies[COMPUTE_DEVICE_BUFFER_FACTOR] = (VkBufferCopy) {
		.srcOffset = pMemory->stagingLayout.factorOffset,
		.dstOffset = 0,
		.size = sizeof(args.factor)
	};

	pMemory->deviceLocalToStagingCopy = (VkBufferCopy) {
		.srcOffset = 0,
		.dstOffset = pMemory->stagingLayout.valuesOffset,
		.size = valuesSize
	};

	pMemory->barriers.transferToComputeMemoryBarriers[COMPUTE_DEVICE_BUFFER_VALUES] = vkToolkitCreateBufferMemoryBarrier(
		pMemory->deviceBuffers[COMPUTE_DEVICE_BUFFER_VALUES].buffer,
		VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		VK_ACCESS_2_TRANSFER_WRITE_BIT,
		VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
		VK_ACCESS_2_SHADER_STORAGE_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT
	);

	pMemory->barriers.transferToComputeMemoryBarriers[COMPUTE_DEVICE_BUFFER_FACTOR] = vkToolkitCreateBufferMemoryBarrier(
		pMemory->deviceBuffers[COMPUTE_DEVICE_BUFFER_FACTOR].buffer,
		VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		VK_ACCESS_2_TRANSFER_WRITE_BIT,
		VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
		VK_ACCESS_2_SHADER_STORAGE_READ_BIT
	);

	pMemory->barriers.computeToTransferMemoryBarrier = vkToolkitCreateBufferMemoryBarrier(
		pMemory->deviceBuffers[COMPUTE_DEVICE_BUFFER_VALUES].buffer,
		VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
		VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
		VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		VK_ACCESS_2_TRANSFER_READ_BIT
	);

	pMemory->barriers.transferToHostMemoryBarrier = vkToolkitCreateBufferMemoryBarrier(
		pMemory->stagingBuffer.buffer,
		VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		VK_ACCESS_2_TRANSFER_WRITE_BIT,
		VK_PIPELINE_STAGE_2_HOST_BIT,
		VK_ACCESS_2_HOST_READ_BIT
	);

	pMemory->barriers.transferToComputeDependencyInfo = (VkDependencyInfo) {
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.pNext = NULL,
		.bufferMemoryBarrierCount = COMPUTE_DEVICE_BUFFER_COUNT,
		.pBufferMemoryBarriers = pMemory->barriers.transferToComputeMemoryBarriers
	};

	pMemory->barriers.computeToTransferDependencyInfo = (VkDependencyInfo) {
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.pNext = NULL,
		.bufferMemoryBarrierCount = 1,
		.pBufferMemoryBarriers = &pMemory->barriers.computeToTransferMemoryBarrier
	};

	pMemory->barriers.transferToHostDependencyInfo = (VkDependencyInfo) {
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.pNext = NULL,
		.bufferMemoryBarrierCount = 1,
		.pBufferMemoryBarriers = &pMemory->barriers.transferToHostMemoryBarrier
	};

	return VK_SUCCESS;
}

void copyBuffers(VkCommandBuffer commandBuffer, const Memory* pMemory) {
	if (commandBuffer == VK_NULL_HANDLE || pMemory == NULL) {
		return;
	}

	for (uint32_t i = 0; i < COMPUTE_DEVICE_BUFFER_COUNT; i++) {
		vkCmdCopyBuffer(
			commandBuffer,
			pMemory->stagingBuffer.buffer,
			pMemory->deviceBuffers[i].buffer,
			1,
			&pMemory->stagingToDeviceLocalCopies[i]
		);
	}
}

void applyBarriers(VkCommandBuffer commandBuffer, const Memory* pMemory) {
	if (commandBuffer == VK_NULL_HANDLE || pMemory == NULL) {
		return;
	}

	vkCmdPipelineBarrier2(commandBuffer, &pMemory->barriers.transferToComputeDependencyInfo);
}
