#ifndef VK_TOOLKIT_DESCRIPTORS_H
#define VK_TOOLKIT_DESCRIPTORS_H

#include <utility>
#include <vector>

#include "bridge.h"



typedef struct VkToolkitCreateDescriptorSetsArgs {
	VkDevice device;
	const VkBridgePipelineReflection* pReflection;
	uint32_t setMultiplier;
	const std::vector<VkDescriptorSetLayout>* pSetLayouts;
	VkDescriptorPool* pPool;
	std::vector<VkDescriptorSet>* pSets;
} VkToolkitCreateDescriptorSetsArgs;

static uint8_t vkToolkitCreateDescriptorSets(VkToolkitCreateDescriptorSetsArgs args) {
	if (
		args.device == VK_NULL_HANDLE || args.pReflection == NULL ||
		args.setMultiplier == 0 || args.pSetLayouts == NULL ||
		args.pPool == NULL || args.pSets == NULL
	) {
		return 0;
	}

	uint32_t maxSets = 0;
	uint32_t poolSizeCount = 0;
	VkBridgeGetPoolSizesArgs poolSizesArgs = {
		.setCount = args.pReflection->descriptorSetCount,
		.pSets = args.pReflection->pDescriptorSets,
		.descriptorCountMultiplier = args.setMultiplier,
		.pMaxSets = &maxSets,
		.pPoolSizeCount = &poolSizeCount,
		.pPoolSizes = NULL
	};
	VK_TOOLKIT_BRIDGE_CHECK(vkBridgeGetPoolSizes(poolSizesArgs), return 0);

	std::vector<VkDescriptorPoolSize> poolSizes(poolSizeCount);
	poolSizesArgs.pPoolSizes = poolSizes.data();
	VK_TOOLKIT_BRIDGE_CHECK(vkBridgeGetPoolSizes(poolSizesArgs), return 0);

	VkDescriptorPoolCreateInfo poolCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.maxSets = maxSets,
		.poolSizeCount = (uint32_t)poolSizes.size(),
		.pPoolSizes = poolSizes.data()
	};

	VkDescriptorPool pool = VK_NULL_HANDLE;
	VkResult result = vkCreateDescriptorPool(args.device, &poolCreateInfo, NULL, &pool);
	VK_TOOLKIT_VK_RESULT_ERROR(result, return 0);

	std::vector<VkDescriptorSetLayout> allocationSetLayouts;
	allocationSetLayouts.reserve(maxSets);
	for (uint32_t copy = 0; copy < args.setMultiplier; copy++) {
		allocationSetLayouts.insert(
			allocationSetLayouts.end(),
			args.pSetLayouts->begin(),
			args.pSetLayouts->end()
		);
	}

	std::vector<VkDescriptorSet> sets(maxSets);
	VkDescriptorSetAllocateInfo allocateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = NULL,
		.descriptorPool = pool,
		.descriptorSetCount = maxSets,
		.pSetLayouts = allocationSetLayouts.data()
	};

	result = vkAllocateDescriptorSets(args.device, &allocateInfo, sets.data());
	if (result != VK_SUCCESS) {
		VK_TOOLKIT_VK_RESULT_ERROR(result, (void)0);
		vkDestroyDescriptorPool(args.device, pool, NULL);
		return 0;
	}

	*args.pPool = pool;
	*args.pSets = std::move(sets);
	return 1;
}

#endif//VK_TOOLKIT_DESCRIPTORS_H
