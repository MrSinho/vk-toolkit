#include "compute.h"

#include "../common/descriptors.h"

#include <utility>


	
uint8_t createDescriptors(CreateDescriptorsArgs args) {
	if (
		args.device == VK_NULL_HANDLE || args.pMemory == NULL ||
		args.pPipeline == NULL || args.pDescriptors == NULL
	) {
		return 0;
	}

	VkBridgeDescriptorSetLayoutBinding valuesBinding = {};
	VkBridgeDescriptorSetLayoutBinding factorBinding = {};

	VK_TOOLKIT_BRIDGE_CHECK(vkBridgeGetSetLayoutBindingByName((VkBridgeGetSetLayoutBindingByNameArgs) {
		.name = "values",
		.setCount = args.pPipeline->pipelineReflection.descriptorSetCount,
		.pSets = args.pPipeline->pipelineReflection.pDescriptorSets,
		.pSetLayoutBinding = &valuesBinding
	}), return 0);

	VK_TOOLKIT_BRIDGE_CHECK(vkBridgeGetSetLayoutBindingByName((VkBridgeGetSetLayoutBindingByNameArgs) {
		.name = "factors",
		.setCount = args.pPipeline->pipelineReflection.descriptorSetCount,
		.pSets = args.pPipeline->pipelineReflection.pDescriptorSets,
		.pSetLayoutBinding = &factorBinding
	}), return 0);

	VkDescriptorPool pool = VK_NULL_HANDLE;
	std::vector<VkDescriptorSet> sets;
	if (!vkToolkitCreateDescriptorSets((VkToolkitCreateDescriptorSetsArgs) {
		.device = args.device,
		.pReflection = &args.pPipeline->pipelineReflection,
		.setMultiplier = 1,
		.pSetLayouts = &args.pPipeline->setLayouts,
		.pPool = &pool,
		.pSets = &sets
	})) {
		return 0;
	}

	VkDescriptorBufferInfo bufferInfos[2] = {
		{
			.buffer = args.pMemory->deviceBuffers[COMPUTE_DEVICE_BUFFER_VALUES].buffer,
			.offset = 0,
			.range = args.pMemory->deviceBuffers[COMPUTE_DEVICE_BUFFER_VALUES].size
		},
		{
			.buffer = args.pMemory->deviceBuffers[COMPUTE_DEVICE_BUFFER_FACTOR].buffer,
			.offset = 0,
			.range = args.pMemory->deviceBuffers[COMPUTE_DEVICE_BUFFER_FACTOR].size
		}
	};

	VkWriteDescriptorSet writes[2] = {
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = sets[valuesBinding.set],
			.dstBinding = valuesBinding.setLayoutBinding.binding,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = valuesBinding.setLayoutBinding.descriptorType,
			.pImageInfo = NULL,
			.pBufferInfo = &bufferInfos[0],
			.pTexelBufferView = NULL
		},
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = sets[factorBinding.set],
			.dstBinding = factorBinding.setLayoutBinding.binding,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = factorBinding.setLayoutBinding.descriptorType,
			.pImageInfo = NULL,
			.pBufferInfo = &bufferInfos[1],
			.pTexelBufferView = NULL
		}
	};

	vkUpdateDescriptorSets(args.device, COMPUTE_ARRAY_COUNT(writes), writes, 0, NULL);

	args.pDescriptors->pool = pool;
	args.pDescriptors->sets = std::move(sets);
	args.pDescriptors->valuesBinding = valuesBinding;
	args.pDescriptors->factorBinding = factorBinding;

	return 1;
}

void destroyDescriptors(VkDevice device, Descriptors* pDescriptors) {
	if (device == VK_NULL_HANDLE || pDescriptors == NULL) {
		return;
	}

	vkDestroyDescriptorPool(device, pDescriptors->pool, NULL);
	*pDescriptors = Descriptors {};
}
