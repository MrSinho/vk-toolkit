#include "scene.h"

#include "../common/memory.h"

#include <string.h>

static void destroyBufferResources(
	VkDevice device,
	VkShellBufferData* pStagingBuffer,
	VkShellBufferMemoryData* pStagingMemory,
	VkShellBufferData* pDeviceLocalBuffers,
	VkShellBufferMemoryData* pDeviceLocalMemory
) {
	if (pStagingMemory->pMapped != NULL) {
		vkUnmapMemory(device, pStagingMemory->memory);
	}
	vkDestroyBuffer(device, pStagingBuffer->buffer, NULL);
	for (uint32_t i = 0; i < SCENE_DEVICE_BUFFER_COUNT; i++) {
		vkDestroyBuffer(device, pDeviceLocalBuffers[i].buffer, NULL);
	}
	vkFreeMemory(device, pStagingMemory->memory, NULL);
	vkFreeMemory(device, pDeviceLocalMemory->memory, NULL);
}

uint8_t createMemory(CreateMemoryArgs args) {
	if (
		args.device == VK_NULL_HANDLE || args.physicalDevice == VK_NULL_HANDLE ||
		args.frameCount == 0 || args.frameCount > SCENE_MAX_FRAMES_IN_FLIGHT ||
		args.pMemory == NULL
	) {
		return 0;
	}

	StagingLayout stagingLayout = {
		.quadVerticesOffset     = 0,
		.triangleVerticesOffset = stagingLayout.quadVerticesOffset     + sizeof(quad),
		.instanceModelsOffset   = stagingLayout.triangleVerticesOffset + args.frameCount * sizeof(triangle),
		.quadIndicesOffset      = stagingLayout.instanceModelsOffset   + sizeof(models),
		.lightOffset            = stagingLayout.quadIndicesOffset      + sizeof(indices),
		.size                   = stagingLayout.lightOffset            + sizeof(light)
	};

	VkShellBufferData stagingBuffer = {};
	VkShellBufferMemoryData stagingMemory = {};
	VkShellBufferData deviceLocalBuffers[SCENE_DEVICE_BUFFER_COUNT] = {};
	VkShellBufferMemoryData deviceLocalMemory = {};

	#define SCENE_MEMORY_CREATE_FAILURE()\
		do {\
			destroyBufferResources(\
				args.device, &stagingBuffer, &stagingMemory, deviceLocalBuffers, &deviceLocalMemory\
			);\
			return 0;\
		} while (0)

	VK_TOOLKIT_VK_RESULT_ERROR(
		createBuffer(
			args.device,
			stagingLayout.size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			&stagingBuffer
		),
		SCENE_MEMORY_CREATE_FAILURE()
	);

	VK_TOOLKIT_VK_RESULT_ERROR(
		vkToolkitAllocateBufferMemory((VkToolkitAllocateBufferMemoryArgs) {
			.device = args.device,
			.physicalDevice = args.physicalDevice,
			.bufferCount = 1,
			.pBuffers = &stagingBuffer,
			.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			.map = 1,
			.pMemory = &stagingMemory
		}),
		SCENE_MEMORY_CREATE_FAILURE()
	);

	uint8_t* pMappedStaging = (uint8_t*)stagingMemory.pMapped;
	memcpy(&pMappedStaging[stagingLayout.quadVerticesOffset],     quad,     sizeof(quad));
	for (uint32_t frame = 0; frame < args.frameCount; frame++) {
		VkDeviceSize triangleOffset = stagingLayout.triangleVerticesOffset + frame * sizeof(triangle);
		memcpy(&pMappedStaging[triangleOffset], triangle, sizeof(triangle));
	}
	memcpy(&pMappedStaging[stagingLayout.instanceModelsOffset],   models,   sizeof(models));
	memcpy(&pMappedStaging[stagingLayout.quadIndicesOffset],      indices,  sizeof(indices));
	memcpy(&pMappedStaging[stagingLayout.lightOffset],            light,    sizeof(light));

	VK_TOOLKIT_VK_RESULT_ERROR(
		createBuffer(
			args.device,
			(VkDeviceSize)(sizeof(quad) + args.frameCount * sizeof(triangle)),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			&deviceLocalBuffers[SCENE_DEVICE_BUFFER_VERTEX]
		),
		SCENE_MEMORY_CREATE_FAILURE()
	);

	VK_TOOLKIT_VK_RESULT_ERROR(
		createBuffer(
			args.device,
			(VkDeviceSize)sizeof(models),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			&deviceLocalBuffers[SCENE_DEVICE_BUFFER_INSTANCE]
		),
		SCENE_MEMORY_CREATE_FAILURE()
	);

	VK_TOOLKIT_VK_RESULT_ERROR(
		createBuffer(
			args.device,
			(VkDeviceSize)sizeof(indices),
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			&deviceLocalBuffers[SCENE_DEVICE_BUFFER_INDEX]
		),
		SCENE_MEMORY_CREATE_FAILURE()
	);

	VK_TOOLKIT_VK_RESULT_ERROR(
		createBuffer(
			args.device,
			(VkDeviceSize)sizeof(light),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			&deviceLocalBuffers[SCENE_DEVICE_BUFFER_UNIFORM]
		),
		SCENE_MEMORY_CREATE_FAILURE()
	);

	VK_TOOLKIT_VK_RESULT_ERROR(
		vkToolkitAllocateBufferMemory((VkToolkitAllocateBufferMemoryArgs) {
			.device = args.device,
			.physicalDevice = args.physicalDevice,
			.bufferCount = ARRAY_COUNT(deviceLocalBuffers),
			.pBuffers = deviceLocalBuffers,
			.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			.map = 0,
			.pMemory = &deviceLocalMemory
		}),
		SCENE_MEMORY_CREATE_FAILURE()
	);

	VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_STATUS, "Staging and device-local memory allocated");

	VkBufferCopy bufferCopies[SCENE_DEVICE_BUFFER_COUNT] = {};

	bufferCopies[SCENE_DEVICE_BUFFER_VERTEX] = (VkBufferCopy) {
		.srcOffset = stagingLayout.quadVerticesOffset,
		.dstOffset = 0,
		.size = sizeof(quad) + args.frameCount * sizeof(triangle)
	};

	bufferCopies[SCENE_DEVICE_BUFFER_INSTANCE] = (VkBufferCopy) {
		.srcOffset = stagingLayout.instanceModelsOffset,
		.dstOffset = 0,
		.size = sizeof(models)
	};

	bufferCopies[SCENE_DEVICE_BUFFER_INDEX] = (VkBufferCopy) {
		.srcOffset = stagingLayout.quadIndicesOffset,
		.dstOffset = 0,
		.size = sizeof(indices)
	};

	bufferCopies[SCENE_DEVICE_BUFFER_UNIFORM] = (VkBufferCopy) {
		.srcOffset = stagingLayout.lightOffset,
		.dstOffset = 0,
		.size = sizeof(light)
	};

	VkBufferCopy triangleBufferCopies[SCENE_MAX_FRAMES_IN_FLIGHT] = {};
	for (uint32_t frame = 0; frame < args.frameCount; frame++) {
		triangleBufferCopies[frame] = (VkBufferCopy) {
			.srcOffset = stagingLayout.triangleVerticesOffset + frame * sizeof(triangle),
			.dstOffset = sizeof(quad) + frame * sizeof(triangle),
			.size = sizeof(triangle)
		};
	}

	VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_STATUS, "Memory initialized");

	args.pMemory->stagingLayout = stagingLayout;
	args.pMemory->stagingBuffer = stagingBuffer;
	args.pMemory->stagingMemory = stagingMemory;
	args.pMemory->stagingMemory.pBuffers = &args.pMemory->stagingBuffer;
	args.pMemory->pMappedStaging = pMappedStaging;
	for (uint32_t i = 0; i < SCENE_DEVICE_BUFFER_COUNT; i++) {
		args.pMemory->deviceLocalBuffers[i] = deviceLocalBuffers[i];
		args.pMemory->bufferCopies[i] = bufferCopies[i];
	}
	for (uint32_t frame = 0; frame < args.frameCount; frame++) {
		args.pMemory->triangleBufferCopies[frame] = triangleBufferCopies[frame];
	}
	args.pMemory->frameCount = args.frameCount;

	args.pMemory->barriers.transferToVertexMemoryBarriers[0] = vkToolkitCreateBufferMemoryBarrier(
		deviceLocalBuffers[SCENE_DEVICE_BUFFER_VERTEX].buffer,
		VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		VK_ACCESS_2_TRANSFER_WRITE_BIT,
		VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
		VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT
	);
	args.pMemory->barriers.transferToVertexMemoryBarriers[1] = vkToolkitCreateBufferMemoryBarrier(
		deviceLocalBuffers[SCENE_DEVICE_BUFFER_INSTANCE].buffer,
		VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		VK_ACCESS_2_TRANSFER_WRITE_BIT,
		VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
		VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT
	);
	args.pMemory->barriers.transferToIndexMemoryBarrier = vkToolkitCreateBufferMemoryBarrier(
		deviceLocalBuffers[SCENE_DEVICE_BUFFER_INDEX].buffer,
		VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		VK_ACCESS_2_TRANSFER_WRITE_BIT,
		VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
		VK_ACCESS_2_INDEX_READ_BIT
	);
	args.pMemory->barriers.transferToUniformMemoryBarrier = vkToolkitCreateBufferMemoryBarrier(
		deviceLocalBuffers[SCENE_DEVICE_BUFFER_UNIFORM].buffer,
		VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		VK_ACCESS_2_TRANSFER_WRITE_BIT,
		VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
		VK_ACCESS_2_UNIFORM_READ_BIT
	);

	args.pMemory->barriers.transferToVertexDependencyInfo = (VkDependencyInfo) {
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.pNext = NULL,
		.bufferMemoryBarrierCount = ARRAY_COUNT(args.pMemory->barriers.transferToVertexMemoryBarriers),
		.pBufferMemoryBarriers = args.pMemory->barriers.transferToVertexMemoryBarriers
	};
	args.pMemory->barriers.transferToIndexDependencyInfo = (VkDependencyInfo) {
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.pNext = NULL,
		.bufferMemoryBarrierCount = 1,
		.pBufferMemoryBarriers = &args.pMemory->barriers.transferToIndexMemoryBarrier
	};
	args.pMemory->barriers.transferToUniformDependencyInfo = (VkDependencyInfo) {
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.pNext = NULL,
		.bufferMemoryBarrierCount = 1,
		.pBufferMemoryBarriers = &args.pMemory->barriers.transferToUniformMemoryBarrier
	};

	args.pMemory->deviceLocalMemory = deviceLocalMemory;
	args.pMemory->deviceLocalMemory.pBuffers = args.pMemory->deviceLocalBuffers;

	#undef SCENE_MEMORY_CREATE_FAILURE

	return 1;
}

void destroyMemory(VkDevice device, Memory* pMemory) {
	if (device == VK_NULL_HANDLE || pMemory == NULL) {
		return;
	}

	if (pMemory->stagingMemory.pMapped != NULL) {
		vkUnmapMemory(device, pMemory->stagingMemory.memory);
	}
	vkDestroyBuffer(device, pMemory->stagingBuffer.buffer, NULL);
	for (uint32_t i = 0; i < SCENE_DEVICE_BUFFER_COUNT; i++) {
		vkDestroyBuffer(device, pMemory->deviceLocalBuffers[i].buffer, NULL);
	}
	vkFreeMemory(device, pMemory->stagingMemory.memory, NULL);
	vkFreeMemory(device, pMemory->deviceLocalMemory.memory, NULL);
	*pMemory = (Memory) {};
}

void copyBuffers(VkCommandBuffer commandBuffer, const Memory* pMemory) {
	if (commandBuffer == VK_NULL_HANDLE || pMemory == NULL) {
		return;
	}

	for (uint32_t i = 0; i < SCENE_DEVICE_BUFFER_COUNT; i++) {
		vkCmdCopyBuffer(
			commandBuffer,
			pMemory->stagingBuffer.buffer,
			pMemory->deviceLocalBuffers[i].buffer,
			1,
			&pMemory->bufferCopies[i]
		);
	}
}

void applyBarriers(VkCommandBuffer commandBuffer, const Memory* pMemory) {
	if (commandBuffer == VK_NULL_HANDLE || pMemory == NULL) {
		return;
	}

	vkCmdPipelineBarrier2(commandBuffer, &pMemory->barriers.transferToVertexDependencyInfo);
	vkCmdPipelineBarrier2(commandBuffer, &pMemory->barriers.transferToIndexDependencyInfo);
	vkCmdPipelineBarrier2(commandBuffer, &pMemory->barriers.transferToUniformDependencyInfo);
}
