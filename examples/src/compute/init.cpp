#include "compute.h"



uint8_t initializeVulkan(Driver* pDriver, Commands* pCommands) {
	if (pDriver == NULL || pCommands == NULL) {
		return 0;
	}

	VkResult result = vkShellCreateInstance((VkShellCreateInstanceArgs) {
		.applicationName = VK_TOOLKIT_APP_NAME,
		.engineName = "vk-toolkit",
		#ifdef DEBUG
		.enableValidationLayers = 1,
		#else
		.enableValidationLayers = 0,
		#endif//DEBUG
		.extensionCount = 0,
		.ppExtensionNames = NULL,
		.apiVersion = VK_API_VERSION_1_4,
		.pInstance = &pDriver->instance
	});
	VK_TOOLKIT_VK_RESULT_ERROR(result, return 0);

	VkShellResult shellResult = vkShellSelectPhysicalDevice((VkShellSelectPhysicalDeviceArgs) {
		.instance = pDriver->instance,
		.surface = VK_NULL_HANDLE,
		.queuesRequirements = VK_QUEUE_COMPUTE_BIT,
		.dynamicRendering = 0,
		.synchronization2 = 1,
		.pPhysicalDevice = &pDriver->physicalDevice,
		.pPhysicalDeviceProperties = &pDriver->physicalDeviceProperties,
		.pPhysicalDeviceFeatures2 = NULL,
		.pPhysicalDeviceMemoryProperties = &pDriver->physicalDeviceMemoryProperties
	});
	VK_TOOLKIT_SHELL_RESULT_ERROR(shellResult, return 0);

	uint32_t computeQueueFamilyIndices[VK_SHELL_MAX_STACK_QUEUE_FAMILY_COUNT] = {};
	shellResult = vkShellGetPhysicalDeviceQueueFamilies((VkShellGetPhysicalDeviceQueueFamiliesArgs) {
		.physicalDevice = pDriver->physicalDevice,
		.surface = VK_NULL_HANDLE,
		.pQueueFamilyCount = NULL,
		.pGraphicsQueueFamilyCount = NULL,
		.pSurfaceQueueFamilyCount = NULL,
		.pComputeQueueFamilyCount = NULL,
		.pTransferQueueFamilyCount = NULL,
		.pGraphicsQueueFamilyIndices = NULL,
		.pSurfaceQueueFamilyIndices = NULL,
		.pComputeQueueFamilyIndices = computeQueueFamilyIndices,
		.pTransferQueueFamilyIndices = NULL,
		.pQueueFamiliesProperties = NULL
	});
	VK_TOOLKIT_SHELL_RESULT_ERROR(shellResult, return 0);

	pCommands->queueFamilyIndex = computeQueueFamilyIndices[0];

	float queuePriority = 1.0f;
	VkDeviceQueueCreateInfo queueCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.queueFamilyIndex = pCommands->queueFamilyIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority
	};
	result = vkShellSetLogicalDevice((VkShellSetLogicalDeviceArgs) {
		.physicalDevice = pDriver->physicalDevice,
		.pDevice = &pDriver->device,
		.extensionCount = 0,
		.ppExtensionNames = NULL,
		.deviceQueueCount = 1,
		.pDeviceQueueInfos = &queueCreateInfo
	});
	VK_TOOLKIT_VK_RESULT_ERROR(result, return 0);

	vkGetDeviceQueue(pDriver->device, pCommands->queueFamilyIndex, 0, &pCommands->queue);

	VkCommandPoolCreateInfo commandPoolCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = pCommands->queueFamilyIndex
	};
	result = vkCreateCommandPool(pDriver->device, &commandPoolCreateInfo, NULL, &pCommands->commandPool);
	VK_TOOLKIT_VK_RESULT_ERROR(result, return 0);

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = pCommands->commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};
	result = vkAllocateCommandBuffers(pDriver->device, &commandBufferAllocateInfo, &pCommands->commandBuffer);
	VK_TOOLKIT_VK_RESULT_ERROR(result, return 0);

	result = vkShellCreateFences(pDriver->device, 1, 0, &pCommands->fence);
	VK_TOOLKIT_VK_RESULT_ERROR(result, return 0);

	return 1;
}
