#ifndef VK_TOOLKIT_BRIDGE_UTILS_H
#define VK_TOOLKIT_BRIDGE_UTILS_H

#include <vk-bridge/vk-bridge.h>

#include "common.h"



#define VK_TOOLKIT_BRIDGE_RESULT_ERROR(result, failureExpression)\
	do {\
		const VkBridgeResult vkToolkitBridgeResult = (VkBridgeResult)(result);\
		if (vkToolkitBridgeResult != VK_BRIDGE_SUCCESS) {\
			VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_ERROR, vkBridgeTranslateResult(vkToolkitBridgeResult));\
			failureExpression;\
		}\
	} while (0)

#define VK_TOOLKIT_BRIDGE_CHECK(functionCall, failureExpression)\
	VK_TOOLKIT_BRIDGE_RESULT_ERROR((functionCall), failureExpression)

#define VK_TOOLKIT_SPV_REFLECT_RESULT_ERROR(result, failureExpression)\
	do {\
		const SpvReflectResult vkToolkitReflectResult = (SpvReflectResult)(result);\
		if (vkToolkitReflectResult != SPV_REFLECT_RESULT_SUCCESS) {\
			VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_ERROR, vkBridgeTranslateSpvReflectResult(vkToolkitReflectResult));\
			failureExpression;\
		}\
	} while (0)

#define VK_TOOLKIT_SPV_REFLECT_CHECK(functionCall, failureExpression)\
	VK_TOOLKIT_SPV_REFLECT_RESULT_ERROR((functionCall), failureExpression)

#endif//VK_TOOLKIT_BRIDGE_UTILS_H
