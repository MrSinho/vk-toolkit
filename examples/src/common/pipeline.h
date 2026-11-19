#ifndef VK_TOOLKIT_PIPELINE_H
#define VK_TOOLKIT_PIPELINE_H

#include <vector>

#include "bridge.h"



static void vkToolkitDestroyPipelineLayout(
	VkDevice device,
	std::vector<VkDescriptorSetLayout>* pSetLayouts,
	VkPipelineLayout* pPipelineLayout
) {
	if (device == VK_NULL_HANDLE || pSetLayouts == NULL || pPipelineLayout == NULL) {
		return;
	}

	vkDestroyPipelineLayout(device, *pPipelineLayout, NULL);
	for (VkDescriptorSetLayout setLayout : *pSetLayouts) {
		vkDestroyDescriptorSetLayout(device, setLayout, NULL);
	}

	*pPipelineLayout = VK_NULL_HANDLE;
	pSetLayouts->clear();
}

typedef struct VkToolkitCreatePipelineLayoutArgs {
	VkDevice device;
	const VkBridgePipelineReflection* pReflection;
	std::vector<VkDescriptorSetLayout>* pSetLayouts;
	VkPipelineLayout* pPipelineLayout;
} VkToolkitCreatePipelineLayoutArgs;

static uint8_t vkToolkitCreatePipelineLayout(VkToolkitCreatePipelineLayoutArgs args) {
	if (
		args.device == VK_NULL_HANDLE || args.pReflection == NULL ||
		args.pSetLayouts == NULL || args.pPipelineLayout == NULL
	) {
		return 0;
	}

	args.pSetLayouts->clear();
	args.pSetLayouts->resize(args.pReflection->descriptorSetCount, VK_NULL_HANDLE);
	*args.pPipelineLayout = VK_NULL_HANDLE;

	for (uint32_t i = 0; i < args.pReflection->descriptorSetCount; i++) {
		VkDescriptorSetLayoutCreateInfo createInfo = {};
		VkBridgeResult bridgeResult = vkBridgeGetDescriptorSetLayoutCreateInfo((VkBridgeGetDescriptorSetLayoutCreateInfoArgs) {
			.pDescriptorSet = &args.pReflection->pDescriptorSets[i],
			.pCreateInfo = &createInfo
		});
		if (bridgeResult != VK_BRIDGE_SUCCESS) {
			VK_TOOLKIT_BRIDGE_RESULT_ERROR(bridgeResult, (void)0);
			vkToolkitDestroyPipelineLayout(args.device, args.pSetLayouts, args.pPipelineLayout);
			return 0;
		}

		VkResult result = vkCreateDescriptorSetLayout(
			args.device,
			&createInfo,
			NULL,
			&(*args.pSetLayouts)[i]
		);
		if (result != VK_SUCCESS) {
			VK_TOOLKIT_VK_RESULT_ERROR(result, (void)0);
			vkToolkitDestroyPipelineLayout(args.device, args.pSetLayouts, args.pPipelineLayout);
			return 0;
		}
	}

	VkPipelineLayoutCreateInfo createInfo = {};
	VkBridgeResult bridgeResult = vkBridgeGetPipelineLayoutCreateInfo((VkBridgeGetPipelineLayoutCreateInfoArgs) {
		.pReflection = args.pReflection,
		.setLayoutCount = (uint32_t)args.pSetLayouts->size(),
		.pSetLayouts = args.pSetLayouts->data(),
		.pCreateInfo = &createInfo
	});
	if (bridgeResult != VK_BRIDGE_SUCCESS) {
		VK_TOOLKIT_BRIDGE_RESULT_ERROR(bridgeResult, (void)0);
		vkToolkitDestroyPipelineLayout(args.device, args.pSetLayouts, args.pPipelineLayout);
		return 0;
	}

	VkResult result = vkCreatePipelineLayout(
		args.device,
		&createInfo,
		NULL,
		args.pPipelineLayout
	);
	if (result != VK_SUCCESS) {
		VK_TOOLKIT_VK_RESULT_ERROR(result, (void)0);
		vkToolkitDestroyPipelineLayout(args.device, args.pSetLayouts, args.pPipelineLayout);
		return 0;
	}

	return 1;
}

#endif//VK_TOOLKIT_PIPELINE_H
