#include "scene.h"

#include "../common/descriptors.h"

#include <utility>

uint8_t createDescriptors(CreateDescriptorsArgs args) {
	if (
		args.device == VK_NULL_HANDLE || args.frameCount == 0 ||
		args.lightBuffer == VK_NULL_HANDLE || args.lightBufferSize == 0 ||
		args.pPipeline == NULL || args.pDescriptors == NULL
	) {
		return 0;
	}

	VkBridgeDescriptorSetLayoutBinding lightBinding = {};
	VK_TOOLKIT_BRIDGE_CHECK(vkBridgeGetSetLayoutBindingByName((VkBridgeGetSetLayoutBindingByNameArgs) {
		.name = "light",
		.setCount = args.pPipeline->pipelineReflection.descriptorSetCount,
		.pSets = args.pPipeline->pipelineReflection.pDescriptorSets,
		.pSetLayoutBinding = &lightBinding
	}), return 0);

	VkDescriptorPool pool = VK_NULL_HANDLE;
	std::vector<VkDescriptorSet> sets;
	if (!vkToolkitCreateDescriptorSets((VkToolkitCreateDescriptorSetsArgs) {
		.device = args.device,
		.pReflection = &args.pPipeline->pipelineReflection,
		.setMultiplier = args.frameCount,
		.pSetLayouts = &args.pPipeline->setLayouts,
		.pPool = &pool,
		.pSets = &sets
	})) {
		return 0;
	}

	VkDescriptorBufferInfo lightBufferInfo = {
		.buffer = args.lightBuffer,
		.offset = 0,
		.range = args.lightBufferSize
	};

	uint32_t frameSetStride = (uint32_t)args.pPipeline->setLayouts.size();
	std::vector<VkWriteDescriptorSet> lightWriteSets(args.frameCount);
	for (uint32_t frame = 0; frame < args.frameCount; frame++) {
		uint32_t setIndex = lightBinding.set + frame * frameSetStride;
		lightWriteSets[frame] = (VkWriteDescriptorSet) {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = sets[setIndex],
			.dstBinding = lightBinding.setLayoutBinding.binding,
			.dstArrayElement = 0,
			.descriptorCount = lightBinding.setLayoutBinding.descriptorCount,
			.descriptorType = lightBinding.setLayoutBinding.descriptorType,
			.pImageInfo = NULL,
			.pBufferInfo = &lightBufferInfo,
			.pTexelBufferView = NULL
		};
	}

	vkUpdateDescriptorSets(args.device, (uint32_t)lightWriteSets.size(), lightWriteSets.data(), 0, NULL);

	args.pDescriptors->pool = pool;
	args.pDescriptors->sets = std::move(sets);
	args.pDescriptors->lightBinding = lightBinding;
	args.pDescriptors->frameSetStride = frameSetStride;
	return 1;
}

void destroyDescriptors(VkDevice device, Descriptors* pDescriptors) {
	if (device == VK_NULL_HANDLE || pDescriptors == NULL) {
		return;
	}

	vkDestroyDescriptorPool(device, pDescriptors->pool, NULL);
	pDescriptors->pool = VK_NULL_HANDLE;
	pDescriptors->sets.clear();
	pDescriptors->lightBinding = (VkBridgeDescriptorSetLayoutBinding) {};
	pDescriptors->frameSetStride = 0;
}
