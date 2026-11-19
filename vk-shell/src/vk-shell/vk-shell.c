#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "vk-shell/vk-shell.h"

#include <memory.h>
#include <string.h>



const char* vkShellTranslateResult(VkShellResult result) {
	switch (result) {
		case VK_SHELL_SUCCESS:
			return "VK_SHELL_SUCCESS";
		case VK_SHELL_ERROR_INVALID_ARGUMENT:
			return "VK_SHELL_ERROR_INVALID_ARGUMENT";
		case VK_SHELL_ERROR_OUT_OF_HOST_MEMORY:
			return "VK_SHELL_ERROR_OUT_OF_HOST_MEMORY";
		case VK_SHELL_ERROR_NOT_FOUND:
			return "VK_SHELL_ERROR_NOT_FOUND";
		case VK_SHELL_ERROR_UNSUPPORTED:
			return "VK_SHELL_ERROR_UNSUPPORTED";
		case VK_SHELL_ERROR_BUFFER_TOO_SMALL:
			return "VK_SHELL_ERROR_BUFFER_TOO_SMALL";
		case VK_SHELL_ERROR_LIMIT_EXCEEDED:
			return "VK_SHELL_ERROR_LIMIT_EXCEEDED";
		case VK_SHELL_ERROR_VULKAN:
			return "VK_SHELL_ERROR_VULKAN";
		default:
			return "Unhandled VkShellResult";
	}
}

VkShellResult vkShellFindValidationLayer(
	const char* validationLayerName,
	uint8_t* pFound
) {
	if (validationLayerName == VK_NULL_HANDLE || pFound == VK_NULL_HANDLE) {
		return VK_SHELL_ERROR_INVALID_ARGUMENT;
	}

	*pFound = 0;

	uint32_t availableLayerCount = 0;
	VkResult result = vkEnumerateInstanceLayerProperties(&availableLayerCount, VK_NULL_HANDLE);
	if (result != VK_SUCCESS) {
		return VK_SHELL_ERROR_VULKAN;
	}
	if (availableLayerCount == 0) {
		return VK_SHELL_SUCCESS;
	}

	VkLayerProperties* pLayerProperties = calloc(availableLayerCount, sizeof(VkLayerProperties));
	if (pLayerProperties == VK_NULL_HANDLE) {
		return VK_SHELL_ERROR_OUT_OF_HOST_MEMORY;
	}

	result = vkEnumerateInstanceLayerProperties(&availableLayerCount, pLayerProperties);
	if (result != VK_SUCCESS) {
		free(pLayerProperties);
		return VK_SHELL_ERROR_VULKAN;
	}

	for (uint32_t i = 0; i < availableLayerCount; i++) {
		if (strcmp(pLayerProperties[i].layerName, validationLayerName) == 0) {
			*pFound = 1;
			break;
		}
	}

	free(pLayerProperties);
	return VK_SHELL_SUCCESS;
}

const char* vkShellTranslateVkResult(
	VkResult vkResult
) {
    switch (vkResult)
    {
    case VK_ERROR_DEVICE_LOST:
        return "VK_ERROR_DEVICE_LOST";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        return "VK_ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT:
        return "VK_ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
        return "VK_ERROR_FORMAT_NOT_SUPPORTED";
    case VK_ERROR_FRAGMENTATION:
        return "VK_ERROR_FRAGMENTATION";
    case VK_ERROR_FRAGMENTED_POOL:
        return "VK_ERROR_FRAGMENTED_POOL";
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
        return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        return "VK_ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_INITIALIZATION_FAILED:
        return "VK_ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
        return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
        return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
    case VK_ERROR_INVALID_SHADER_NV:
        return "VK_ERROR_INVALID_SHADER_NV";
    case VK_ERROR_LAYER_NOT_PRESENT:
        return "VK_ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_MEMORY_MAP_FAILED:
        return "VK_ERROR_MEMORY_MAP_FAILED";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
    case VK_ERROR_NOT_PERMITTED_EXT:
        return "VK_ERROR_NOT_PERMITTED_EXT";
    case VK_ERROR_OUT_OF_DATE_KHR:
        return "VK_ERROR_OUT_OF_DATE_KHR";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        return "VK_ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_POOL_MEMORY:
        return "VK_ERROR_OUT_OF_POOL_MEMORY";
    case VK_ERROR_SURFACE_LOST_KHR:
        return "VK_ERROR_SURFACE_LOST_KHR";
    case VK_ERROR_TOO_MANY_OBJECTS:
        return "VK_ERROR_TOO_MANY_OBJECTS";
    case VK_ERROR_UNKNOWN:
        return "VK_ERROR_UNKNOWN";
    case VK_ERROR_VALIDATION_FAILED_EXT:
        return "VK_ERROR_VALIDATION_FAILED_EXT";
    case VK_EVENT_RESET:
        return "VK_EVENT_RESET";
    case VK_EVENT_SET:
        return "VK_EVENT_SET";
    case VK_INCOMPLETE:
        return "VK_INCOMPLETE";
    case VK_NOT_READY:
        return "VK_NOT_READY";
#if 0//not supported by lavapipe
    case VK_OPERATION_DEFERRED_KHR:
        return "VK_OPERATION_DEFERRED_KHR";
    case VK_OPERATION_NOT_DEFERRED_KHR:
        return "VK_OPERATION_NOT_DEFERRED_KHR";
    case VK_PIPELINE_COMPILE_REQUIRED_EXT:
        return "VK_PIPELINE_COMPILE_REQUIRED_EXT";
#endif//0
    case VK_SUBOPTIMAL_KHR:
        return "VK_SUBOPTIMAL_KHR";
    case VK_SUCCESS:
        return "VK_SUCCESS";
#if 0//not supported by lavapipe
    case VK_THREAD_DONE_KHR:
        return "VK_THREAD_DONE_KHR";
    case VK_THREAD_IDLE_KHR:
        return "VK_THREAD_IDLE_KHR";
#endif//0
    case VK_TIMEOUT:
        return "VK_TIMEOUT";
    default:
        return "Unhandled VkResult";
    }
}

VkResult vkShellCreateInstance(VkShellCreateInstanceArgs args) {
	if (args.pInstance == VK_NULL_HANDLE || args.applicationName == VK_NULL_HANDLE || args.engineName == VK_NULL_HANDLE) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}
	if (args.extensionCount > 0 && args.ppExtensionNames == VK_NULL_HANDLE) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkApplicationInfo applicationInfo = {
		.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext              = VK_NULL_HANDLE,
		.pApplicationName   = args.applicationName,
		.applicationVersion = VK_MAKE_VERSION(0, 1, 0),
		.pEngineName        = args.engineName,
		.engineVersion      = VK_MAKE_VERSION(0, 1, 0),
		.apiVersion         = args.apiVersion,
	};

	VkInstanceCreateInfo instanceCreateInfo = {
		.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext                   = VK_NULL_HANDLE,
		.flags                   = 0,
		.pApplicationInfo        = &applicationInfo,
		.enabledLayerCount       = 0,
		.ppEnabledLayerNames     = VK_NULL_HANDLE,
		.enabledExtensionCount   = args.extensionCount,
		.ppEnabledExtensionNames = args.ppExtensionNames
	};

	const char* khronosValidation = "VK_LAYER_KHRONOS_validation";
	if (args.enableValidationLayers) {
		uint8_t validationLayerFound = 0;
		VkShellResult findResult = vkShellFindValidationLayer(khronosValidation, &validationLayerFound);
		if (findResult != VK_SHELL_SUCCESS) {
			return VK_ERROR_INITIALIZATION_FAILED;
		}
		if (validationLayerFound) {
			instanceCreateInfo.enabledLayerCount = 1;
			instanceCreateInfo.ppEnabledLayerNames = &khronosValidation;
		}
	}

	VkResult result = vkCreateInstance(&instanceCreateInfo, VK_NULL_HANDLE, args.pInstance);
	if (result != VK_SUCCESS) {
		return result;
	}

	return VK_SUCCESS;
}

VkShellResult vkShellGetPhysicalDeviceQueueFamilies(VkShellGetPhysicalDeviceQueueFamiliesArgs args) {
	if (args.physicalDevice == VK_NULL_HANDLE) {
		return VK_SHELL_ERROR_INVALID_ARGUMENT;
	}

	uint32_t                queueFamilyCount = 0;
	VkQueueFamilyProperties queueFamiliesProperties[VK_SHELL_MAX_STACK_QUEUE_FAMILY_COUNT];

	vkGetPhysicalDeviceQueueFamilyProperties(args.physicalDevice, &queueFamilyCount, VK_NULL_HANDLE);
	if (queueFamilyCount > VK_SHELL_MAX_STACK_QUEUE_FAMILY_COUNT) {
		return VK_SHELL_ERROR_LIMIT_EXCEEDED;
	}
	vkGetPhysicalDeviceQueueFamilyProperties(args.physicalDevice, &queueFamilyCount, queueFamiliesProperties);

	uint32_t graphicsQueueFamilyCount  = 0;
	uint32_t surfaceQueueFamilyCount   = 0;
	uint32_t computeQueueFamilyCount   = 0;
	uint32_t transferQueueFamilyCount  = 0;
			 
	uint32_t graphicsQueueFamilyIndices[VK_SHELL_MAX_STACK_QUEUE_FAMILY_COUNT]  = { 0 };
	uint32_t surfaceQueueFamilyIndices [VK_SHELL_MAX_STACK_QUEUE_FAMILY_COUNT]  = { 0 };
	uint32_t computeQueueFamilyIndices [VK_SHELL_MAX_STACK_QUEUE_FAMILY_COUNT]  = { 0 };
	uint32_t transferQueueFamilyIndices [VK_SHELL_MAX_STACK_QUEUE_FAMILY_COUNT] = { 0 };
	
	for (uint32_t queueFamilyIdx = 0; queueFamilyIdx < queueFamilyCount; queueFamilyIdx++) {
			
		if (args.surface != VK_NULL_HANDLE) {
			uint32_t surfaceSupport = 0;
			VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(args.physicalDevice, queueFamilyIdx, args.surface, &surfaceSupport);
			if (result != VK_SUCCESS) {
				return VK_SHELL_ERROR_VULKAN;
			}
			if (surfaceSupport) {
				surfaceQueueFamilyIndices[surfaceQueueFamilyCount] = queueFamilyIdx;
				surfaceQueueFamilyCount++;
			}
		}
		
		if (queueFamiliesProperties[queueFamilyIdx].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			graphicsQueueFamilyIndices[graphicsQueueFamilyCount] = queueFamilyIdx;
			graphicsQueueFamilyCount++;
		}

		if (queueFamiliesProperties[queueFamilyIdx].queueFlags & VK_QUEUE_COMPUTE_BIT) {
			computeQueueFamilyIndices[computeQueueFamilyCount] = queueFamilyIdx;
			computeQueueFamilyCount++;
		}

		if (queueFamiliesProperties[queueFamilyIdx].queueFlags & VK_QUEUE_TRANSFER_BIT) {
			transferQueueFamilyIndices[transferQueueFamilyCount] = queueFamilyIdx;
			transferQueueFamilyCount++;
		}

	}
	
	if (args.pQueueFamilyCount != VK_NULL_HANDLE) {
		(*args.pQueueFamilyCount) = queueFamilyCount;
	}
	if (args.pGraphicsQueueFamilyCount != VK_NULL_HANDLE) {
		(*args.pGraphicsQueueFamilyCount) = graphicsQueueFamilyCount;
	}
	if (args.pSurfaceQueueFamilyCount) {
		(*args.pSurfaceQueueFamilyCount) = surfaceQueueFamilyCount;
	}
	if (args.pComputeQueueFamilyCount != VK_NULL_HANDLE) {
		(*args.pComputeQueueFamilyCount) = computeQueueFamilyCount;
	}
	if (args.pTransferQueueFamilyCount != VK_NULL_HANDLE) {
		(*args.pTransferQueueFamilyCount) = transferQueueFamilyCount;
	}
	if (args.pGraphicsQueueFamilyIndices) {
		memcpy(args.pGraphicsQueueFamilyIndices, graphicsQueueFamilyIndices, graphicsQueueFamilyCount * sizeof(uint32_t));
	}
	if (args.pSurfaceQueueFamilyIndices) {
		memcpy(args.pSurfaceQueueFamilyIndices, surfaceQueueFamilyIndices, surfaceQueueFamilyCount * sizeof(uint32_t));
	}
	if (args.pComputeQueueFamilyIndices) {
		memcpy(args.pComputeQueueFamilyIndices, computeQueueFamilyIndices, computeQueueFamilyCount * sizeof(uint32_t));
	}
	if (args.pTransferQueueFamilyIndices) {
		memcpy(args.pTransferQueueFamilyIndices, transferQueueFamilyIndices, transferQueueFamilyCount * sizeof(uint32_t));
	}
	if (args.pQueueFamiliesProperties) {
		memcpy(args.pQueueFamiliesProperties, queueFamiliesProperties, queueFamilyCount * sizeof(VkQueueFamilyProperties));
	}

	return VK_SHELL_SUCCESS;
}

VkShellResult vkShellSelectPhysicalDevice(VkShellSelectPhysicalDeviceArgs args) {
	if (args.instance == VK_NULL_HANDLE || args.pPhysicalDevice == VK_NULL_HANDLE || args.queuesRequirements == 0) {
		return VK_SHELL_ERROR_INVALID_ARGUMENT;
	}

	uint32_t physicalDeviceCount = 0;
	VkResult result = vkEnumeratePhysicalDevices(args.instance, &physicalDeviceCount, VK_NULL_HANDLE);
	if (result != VK_SUCCESS) {
		return VK_SHELL_ERROR_VULKAN;
	}
	if (physicalDeviceCount == 0) {
		return VK_SHELL_ERROR_NOT_FOUND;
	}
	if (physicalDeviceCount > VK_SHELL_MAX_STACK_PHYSICAL_DEVICE_COUNT) {
		return VK_SHELL_ERROR_LIMIT_EXCEEDED;
	}

	VkPhysicalDevice physicalDevices[VK_SHELL_MAX_STACK_PHYSICAL_DEVICE_COUNT] = { 0 };
	result = vkEnumeratePhysicalDevices(args.instance, &physicalDeviceCount, physicalDevices);
	if (result != VK_SUCCESS) {
		return VK_SHELL_ERROR_VULKAN;
	}

	VkPhysicalDevice suitablePhysicalDevices[VK_SHELL_MAX_STACK_PHYSICAL_DEVICE_COUNT] = { 0 };
	uint32_t suitablePhysicalDeviceCount = 0;

	for (uint32_t physicalDeviceIdx = 0; physicalDeviceIdx < physicalDeviceCount; physicalDeviceIdx++) {
		uint32_t graphicsQueueFamilyCount = 0;
		uint32_t surfaceQueueFamilyCount  = 0;
		uint32_t computeQueueFamilyCount  = 0;
		uint32_t transferQueueFamilyCount = 0;

		VkShellResult queueResult = vkShellGetPhysicalDeviceQueueFamilies((VkShellGetPhysicalDeviceQueueFamiliesArgs) {
			.physicalDevice               = physicalDevices[physicalDeviceIdx],
			.surface                      = args.surface,
			.pGraphicsQueueFamilyCount   = &graphicsQueueFamilyCount,
			.pSurfaceQueueFamilyCount    = &surfaceQueueFamilyCount,
			.pComputeQueueFamilyCount    = &computeQueueFamilyCount,
			.pTransferQueueFamilyCount   = &transferQueueFamilyCount
		});
		if (queueResult != VK_SHELL_SUCCESS) {
			return queueResult;
		}

		uint8_t graphicsSupported = (args.queuesRequirements & VK_QUEUE_GRAPHICS_BIT) == 0 || graphicsQueueFamilyCount > 0;
		uint8_t surfaceSupported  = args.surface == VK_NULL_HANDLE || surfaceQueueFamilyCount > 0;
		uint8_t computeSupported  = (args.queuesRequirements & VK_QUEUE_COMPUTE_BIT) == 0 || computeQueueFamilyCount > 0;
		uint8_t transferSupported = (args.queuesRequirements & VK_QUEUE_TRANSFER_BIT) == 0 || transferQueueFamilyCount > 0;

		VkPhysicalDeviceVulkan11Features vulkan11Features = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
			.pNext = VK_NULL_HANDLE
		};
		VkPhysicalDeviceVulkan13Features vulkan13Features = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
			.pNext = &vulkan11Features
		};
		VkPhysicalDeviceFeatures2 deviceFeatures2 = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
			.pNext = &vulkan13Features
		};
		vkGetPhysicalDeviceFeatures2(physicalDevices[physicalDeviceIdx], &deviceFeatures2);

		uint8_t requiredFeaturesSupported = vulkan11Features.shaderDrawParameters &&
			(!args.dynamicRendering || vulkan13Features.dynamicRendering) &&
			(!args.synchronization2 || vulkan13Features.synchronization2);

		if (graphicsSupported && surfaceSupported && computeSupported && transferSupported && requiredFeaturesSupported) {
			suitablePhysicalDevices[suitablePhysicalDeviceCount++] = physicalDevices[physicalDeviceIdx];
		}
	}

	if (suitablePhysicalDeviceCount == 0) {
		return VK_SHELL_ERROR_NOT_FOUND;
	}

	uint32_t bestScore = 0;
	uint32_t bestDeviceIdx = 0;
	for (uint32_t i = 0; i < suitablePhysicalDeviceCount; i++) {
		VkPhysicalDeviceProperties properties = { 0 };
		vkGetPhysicalDeviceProperties(suitablePhysicalDevices[i], &properties);

		uint32_t score = properties.limits.maxMemoryAllocationCount;
		if (args.queuesRequirements & VK_QUEUE_COMPUTE_BIT) {
			score += properties.limits.maxComputeSharedMemorySize;
			score += properties.limits.maxComputeWorkGroupInvocations;
		}
		if (args.queuesRequirements & VK_QUEUE_GRAPHICS_BIT) {
			score += properties.limits.maxVertexInputAttributes;
			score += properties.limits.maxVertexInputBindings;
			score += properties.limits.maxVertexOutputComponents;
		}

		if (i == 0 || score > bestScore) {
			bestScore = score;
			bestDeviceIdx = i;
		}
	}

	*args.pPhysicalDevice = suitablePhysicalDevices[bestDeviceIdx];
	if (args.pPhysicalDeviceProperties != VK_NULL_HANDLE) {
		vkGetPhysicalDeviceProperties(*args.pPhysicalDevice, args.pPhysicalDeviceProperties);
	}
	if (args.pPhysicalDeviceFeatures2 != VK_NULL_HANDLE) {
		args.pPhysicalDeviceFeatures2->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		vkGetPhysicalDeviceFeatures2(*args.pPhysicalDevice, args.pPhysicalDeviceFeatures2);
	}
	if (args.pPhysicalDeviceMemoryProperties != VK_NULL_HANDLE) {
		vkGetPhysicalDeviceMemoryProperties(*args.pPhysicalDevice, args.pPhysicalDeviceMemoryProperties);
	}

	return VK_SHELL_SUCCESS;
}

VkResult vkShellGetPhysicalDeviceSurfaceCapabilities(
	VkPhysicalDevice physicalDevice,
	VkSurfaceKHR surface,
	VkSurfaceCapabilitiesKHR* pSurfaceCapabilities,
	uint32_t clampCurrentExtentWidthValue,
	uint32_t clampCurrentExtentHeightValue
) {
	if (physicalDevice == VK_NULL_HANDLE || surface == VK_NULL_HANDLE || pSurfaceCapabilities == VK_NULL_HANDLE) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities);
	if (result != VK_SUCCESS) {
		return result;
	}

	if (
		(pSurfaceCapabilities->currentExtent.width == UINT32_MAX) !=
		(pSurfaceCapabilities->currentExtent.height == UINT32_MAX)
	) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (pSurfaceCapabilities->currentExtent.width == UINT32_MAX) {
		if (clampCurrentExtentWidthValue < pSurfaceCapabilities->minImageExtent.width) {
			pSurfaceCapabilities->currentExtent.width = pSurfaceCapabilities->minImageExtent.width;
		}
		else if (clampCurrentExtentWidthValue > pSurfaceCapabilities->maxImageExtent.width) {
			pSurfaceCapabilities->currentExtent.width = pSurfaceCapabilities->maxImageExtent.width;
		}
		else {
			pSurfaceCapabilities->currentExtent.width = clampCurrentExtentWidthValue;
		}

		if (clampCurrentExtentHeightValue < pSurfaceCapabilities->minImageExtent.height) {
			pSurfaceCapabilities->currentExtent.height = pSurfaceCapabilities->minImageExtent.height;
		}
		else if (clampCurrentExtentHeightValue > pSurfaceCapabilities->maxImageExtent.height) {
			pSurfaceCapabilities->currentExtent.height = pSurfaceCapabilities->maxImageExtent.height;
		}
		else {
			pSurfaceCapabilities->currentExtent.height = clampCurrentExtentHeightValue;
		}
	}

	return VK_SUCCESS;
}

VkResult vkShellSetLogicalDevice(VkShellSetLogicalDeviceArgs args) {
	if (args.physicalDevice == VK_NULL_HANDLE || args.pDevice == VK_NULL_HANDLE || args.deviceQueueCount == 0 || args.pDeviceQueueInfos == VK_NULL_HANDLE) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}
	if (args.extensionCount > 0 && args.ppExtensionNames == VK_NULL_HANDLE) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkPhysicalDeviceVulkan11Features vulkan11Features = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
		.pNext = VK_NULL_HANDLE
	};
	VkPhysicalDeviceVulkan13Features vulkan13Features = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
		.pNext = &vulkan11Features
	};
	VkPhysicalDeviceFeatures2 deviceFeatures2 = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
		.pNext = &vulkan13Features
	};
	vkGetPhysicalDeviceFeatures2(args.physicalDevice, &deviceFeatures2);

	VkDeviceCreateInfo deviceCreateInfo = {
		.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext                   = &vulkan13Features,
		.flags                   = 0,
		.queueCreateInfoCount    = args.deviceQueueCount,
		.pQueueCreateInfos       = args.pDeviceQueueInfos,
		.enabledLayerCount       = 0,
		.ppEnabledLayerNames     = VK_NULL_HANDLE,
		.enabledExtensionCount   = args.extensionCount,
		.ppEnabledExtensionNames = args.ppExtensionNames,
		.pEnabledFeatures        = VK_NULL_HANDLE
	};

	VkResult result = vkCreateDevice(args.physicalDevice, &deviceCreateInfo, VK_NULL_HANDLE, args.pDevice);
	if (result != VK_SUCCESS) {
		return result;
	}
	return VK_SUCCESS;
}

VkShellResult vkShellCheckSupportedDeviceColorFormat(
	VkPhysicalDevice physicalDevice,
	VkFormat         format,
	uint8_t*         pColorAttachmentSupported
) {
	if (physicalDevice == NULL || format == VK_FORMAT_UNDEFINED || pColorAttachmentSupported == NULL) {
		return VK_SHELL_ERROR_INVALID_ARGUMENT;
	}

	*pColorAttachmentSupported = 0;

	VkFormatProperties formatProperties = { 0 };

	vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

	if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) {
		(*pColorAttachmentSupported) = 1;
	}
	
	return VK_SHELL_SUCCESS;
}

VkShellResult vkShellFindSupportedDeviceColorFormats(
	VkPhysicalDevice        physicalDevice,
	uint32_t                minChannelCount,
	uint32_t                maxChannelCount,
	uint32_t                minChannelSize,
	uint32_t                maxChannelSize,
	VkShellImageChannelTypeFlags channelTypes,
	uint32_t*               pSupportedFormatCount,
	VkFormat*               pSupportedFormats,
	uint32_t*               pChannelsCount,
	uint32_t*               pSingleChannelsSizes,
	uint32_t*               pChannelsTypes
) {
	if (
		physicalDevice == NULL ||
		pSupportedFormatCount == NULL ||
		pSupportedFormats == NULL ||
		pSingleChannelsSizes == NULL ||
		pChannelsCount == NULL ||
		pChannelsTypes == NULL
	) {
		return VK_SHELL_ERROR_INVALID_ARGUMENT;
	}
	if (minChannelCount > maxChannelCount || minChannelSize > maxChannelSize || channelTypes == 0) {
		return VK_SHELL_ERROR_INVALID_ARGUMENT;
	}

	VkFormat colorFormats[VK_SHELL_MAX_STACK_DEVICE_COLOR_FORMATS_QUERIES] = {
		VK_FORMAT_R8_UNORM,
		VK_FORMAT_R8G8_UNORM,
		VK_FORMAT_R8G8B8_UNORM,
		VK_FORMAT_R8G8B8A8_UNORM,

		VK_FORMAT_R8_SNORM,
		VK_FORMAT_R8G8_SNORM,
		VK_FORMAT_R8G8B8_SNORM,
		VK_FORMAT_R8G8B8A8_SNORM,


		VK_FORMAT_R16_UNORM,
		VK_FORMAT_R16G16_UNORM,
		VK_FORMAT_R16G16B16_UNORM,
		VK_FORMAT_R16G16B16A16_UNORM,

		VK_FORMAT_R16_SNORM,
		VK_FORMAT_R16G16_SNORM,
		VK_FORMAT_R16G16B16_SNORM,
		VK_FORMAT_R16G16B16A16_SNORM,

		VK_FORMAT_R16_SFLOAT,
		VK_FORMAT_R16G16_SFLOAT,
		VK_FORMAT_R16G16B16_SFLOAT,
		VK_FORMAT_R16G16B16A16_SFLOAT,


		VK_FORMAT_R32_UINT,
		VK_FORMAT_R32G32_UINT,
		VK_FORMAT_R32G32B32_UINT,
		VK_FORMAT_R32G32B32A32_UINT,

		VK_FORMAT_R32_SINT,
		VK_FORMAT_R32G32_SINT,
		VK_FORMAT_R32G32B32_SINT,
		VK_FORMAT_R32G32B32A32_SINT,

		VK_FORMAT_R32_SFLOAT,
		VK_FORMAT_R32G32_SFLOAT,
		VK_FORMAT_R32G32B32_SFLOAT,
		VK_FORMAT_R32G32B32A32_SFLOAT,


		VK_FORMAT_R64_UINT,
		VK_FORMAT_R64G64_UINT,
		VK_FORMAT_R64G64B64_UINT,
		VK_FORMAT_R64G64B64A64_UINT,

		VK_FORMAT_R64_SINT,
		VK_FORMAT_R64G64_SINT,
		VK_FORMAT_R64G64B64_SINT,
		VK_FORMAT_R64G64B64A64_SINT,

		VK_FORMAT_R64_SFLOAT,
		VK_FORMAT_R64G64_SFLOAT,
		VK_FORMAT_R64G64B64_SFLOAT,
		VK_FORMAT_R64G64B64A64_SFLOAT,
	};


	
	uint32_t channelsSizes[VK_SHELL_MAX_STACK_DEVICE_COLOR_FORMATS_QUERIES] = {
		1,//VK_FORMAT_R8_UNORM
		1,//VK_FORMAT_R8G8_UNORM
		1,//VK_FORMAT_R8G8B8_UNORM
		1,//VK_FORMAT_R8G8B8A8_UNORM
	
		1,//VK_FORMAT_R8_SNORM
		1,//VK_FORMAT_R8G8_SNORM
		1,//VK_FORMAT_R8G8B8_SNORM
		1,//VK_FORMAT_R8G8B8A8_SNORM
	
	
		2,//VK_FORMAT_R16_UINT
		2,//VK_FORMAT_R16G16_UINT
		2,//VK_FORMAT_R16G16B16_UINT
		2,//VK_FORMAT_R16G16B16A16_UINT
	
		2,//VK_FORMAT_R16_SINT
		2,//VK_FORMAT_R16G16_SINT
		2,//VK_FORMAT_R16G16B16_SINT
		2,//VK_FORMAT_R16G16B16A16_SINT
	
		2,//VK_FORMAT_R16_SFLOAT
		2,//VK_FORMAT_R16G16_SFLOAT
		2,//VK_FORMAT_R16G16B16_SFLOAT
		2,//VK_FORMAT_R16G16B16A16_SFLOAT
	
	
		4,//VK_FORMAT_R32_UINT
		4,//VK_FORMAT_R32G32_UINT
		4,//VK_FORMAT_R32G32B32_UINT
		4,//VK_FORMAT_R32G32B32A32_UINT
	
		4,//VK_FORMAT_R32_SINT
		4,//VK_FORMAT_R32G32_SINT
		4,//VK_FORMAT_R32G32B32_SINT
		4,//VK_FORMAT_R32G32B32A32_SINT
	
		4,//VK_FORMAT_R32_SFLOAT
		4,//VK_FORMAT_R32G32_SFLOAT
		4,//VK_FORMAT_R32G32B32_SFLOAT
		4,//VK_FORMAT_R32G32B32A32_SFLOAT
	
	
		8,//VK_FORMAT_R64_UINT
		8,//VK_FORMAT_R64G64_UINT
		8,//VK_FORMAT_R64G64B64_UINT
		8,//VK_FORMAT_R64G64B64A64_UINT
	
		8,//VK_FORMAT_R64_SINT
		8,//VK_FORMAT_R64G64_SINT
		8,//VK_FORMAT_R64G64B64_SINT
		8,//VK_FORMAT_R64G64B64A64_SINT
	
		8,//VK_FORMAT_R64_SFLOAT
		8,//VK_FORMAT_R64G64_SFLOAT
		8,//VK_FORMAT_R64G64B64_SFLOAT
		8,//VK_FORMAT_R64G64B64A64_SFLOAT
	};
	
	uint32_t channelsCount[VK_SHELL_MAX_STACK_DEVICE_COLOR_FORMATS_QUERIES] = {
		1,//VK_FORMAT_R8_UNORM
		2,//VK_FORMAT_R8G8_UNORM
		3,//VK_FORMAT_R8G8B8_UNORM
		4,//VK_FORMAT_R8G8B8A8_UNORM
	
		1,//VK_FORMAT_R8_SNORM
		2,//VK_FORMAT_R8G8_SNORM
		3,//VK_FORMAT_R8G8B8_SNORM
		4,//VK_FORMAT_R8G8B8A8_SNORM
	
	
		1,//VK_FORMAT_R16_UINT
		2,//VK_FORMAT_R16G16_UINT
		3,//VK_FORMAT_R16G16B16_UINT
		4,//VK_FORMAT_R16G16B16A16_UINT
	
		1,//VK_FORMAT_R16_SINT
		2,//VK_FORMAT_R16G16_SINT
		3,//VK_FORMAT_R16G16B16_SINT
		4,//VK_FORMAT_R16G16B16A16_SINT
	
		1,//VK_FORMAT_R16_SFLOAT
		2,//VK_FORMAT_R16G16_SFLOAT
		3,//VK_FORMAT_R16G16B16_SFLOAT
		4,//VK_FORMAT_R16G16B16A16_SFLOAT
	
	
		1,//VK_FORMAT_R32_UINT
		2,//VK_FORMAT_R32G32_UINT
		3,//VK_FORMAT_R32G32B32_UINT
		4,//VK_FORMAT_R32G32B32A32_UINT
	
		1,//VK_FORMAT_R32_SINT
		2,//VK_FORMAT_R32G32_SINT
		3,//VK_FORMAT_R32G32B32_SINT
		4,//VK_FORMAT_R32G32B32A32_SINT
	
		1,//VK_FORMAT_R32_SFLOAT
		2,//VK_FORMAT_R32G32_SFLOAT
		3,//VK_FORMAT_R32G32B32_SFLOAT
		4,//VK_FORMAT_R32G32B32A32_SFLOAT
	
	
		1,//VK_FORMAT_R64_UINT
		2,//VK_FORMAT_R64G64_UINT
		3,//VK_FORMAT_R64G64B64_UINT
		4,//VK_FORMAT_R64G64B64A64_UINT
	
		1,//VK_FORMAT_R64_SINT
		2,//VK_FORMAT_R64G64_SINT
		3,//VK_FORMAT_R64G64B64_SINT
		4,//VK_FORMAT_R64G64B64A64_SINT
	
		1,//VK_FORMAT_R64_SFLOAT
		2,//VK_FORMAT_R64G64_SFLOAT
		3,//VK_FORMAT_R64G64B64_SFLOAT
		4,//VK_FORMAT_R64G64B64A64_SFLOAT
	};

	uint32_t channelsTypes[VK_SHELL_MAX_STACK_DEVICE_COLOR_FORMATS_QUERIES] = {
		VK_SHELL_IMAGE_CHANNEL_TYPE_UNORM,//VK_FORMAT_R8_UNORM
		VK_SHELL_IMAGE_CHANNEL_TYPE_UNORM,//VK_FORMAT_R8G8_UNORM
		VK_SHELL_IMAGE_CHANNEL_TYPE_UNORM,//VK_FORMAT_R8G8B8_UNORM
		VK_SHELL_IMAGE_CHANNEL_TYPE_UNORM,//VK_FORMAT_R8G8B8A8_UNORM
	
		VK_SHELL_IMAGE_CHANNEL_TYPE_SNORM,//VK_FORMAT_R8_SNORM
		VK_SHELL_IMAGE_CHANNEL_TYPE_SNORM,//VK_FORMAT_R8G8_SNORM
		VK_SHELL_IMAGE_CHANNEL_TYPE_SNORM,//VK_FORMAT_R8G8B8_SNORM
		VK_SHELL_IMAGE_CHANNEL_TYPE_SNORM,//VK_FORMAT_R8G8B8A8_SNORM
	

		VK_SHELL_IMAGE_CHANNEL_TYPE_UNORM,//VK_FORMAT_R16_UNORM
		VK_SHELL_IMAGE_CHANNEL_TYPE_UNORM,//VK_FORMAT_R16G16_UNORM
		VK_SHELL_IMAGE_CHANNEL_TYPE_UNORM,//VK_FORMAT_R16G16B16_UNORM
		VK_SHELL_IMAGE_CHANNEL_TYPE_UNORM,//VK_FORMAT_R16G16B16A16_UNORM
	
		VK_SHELL_IMAGE_CHANNEL_TYPE_SNORM,//VK_FORMAT_R16_SNORM
		VK_SHELL_IMAGE_CHANNEL_TYPE_SNORM,//VK_FORMAT_R16G16_SNORM
		VK_SHELL_IMAGE_CHANNEL_TYPE_SNORM,//VK_FORMAT_R16G16B16_SNORM
		VK_SHELL_IMAGE_CHANNEL_TYPE_SNORM,//VK_FORMAT_R16G16B16A16_SNORM
	
		VK_SHELL_IMAGE_CHANNEL_TYPE_SFLOAT,//VK_FORMAT_R16_SFLOAT
		VK_SHELL_IMAGE_CHANNEL_TYPE_SFLOAT,//VK_FORMAT_R16G16_SFLOAT
		VK_SHELL_IMAGE_CHANNEL_TYPE_SFLOAT,//VK_FORMAT_R16G16B16_SFLOAT
		VK_SHELL_IMAGE_CHANNEL_TYPE_SFLOAT,//VK_FORMAT_R16G16B16A16_SFLOAT
	
	
		VK_SHELL_IMAGE_CHANNEL_TYPE_UINT,//VK_FORMAT_R32_UINT
		VK_SHELL_IMAGE_CHANNEL_TYPE_UINT,//VK_FORMAT_R32G32_UINT
		VK_SHELL_IMAGE_CHANNEL_TYPE_UINT,//VK_FORMAT_R32G32B32_UINT
		VK_SHELL_IMAGE_CHANNEL_TYPE_UINT,//VK_FORMAT_R32G32B32A32_UINT
	
		VK_SHELL_IMAGE_CHANNEL_TYPE_SINT,//VK_FORMAT_R32_SINT
		VK_SHELL_IMAGE_CHANNEL_TYPE_SINT,//VK_FORMAT_R32G32_SINT
		VK_SHELL_IMAGE_CHANNEL_TYPE_SINT,//VK_FORMAT_R32G32B32_SINT
		VK_SHELL_IMAGE_CHANNEL_TYPE_SINT,//VK_FORMAT_R32G32B32A32_SINT
	
		VK_SHELL_IMAGE_CHANNEL_TYPE_SFLOAT,//VK_FORMAT_R32_SFLOAT
		VK_SHELL_IMAGE_CHANNEL_TYPE_SFLOAT,//VK_FORMAT_R32G32_SFLOAT
		VK_SHELL_IMAGE_CHANNEL_TYPE_SFLOAT,//VK_FORMAT_R32G32B32_SFLOAT
		VK_SHELL_IMAGE_CHANNEL_TYPE_SFLOAT,//VK_FORMAT_R32G32B32A32_SFLOAT
	
	
		VK_SHELL_IMAGE_CHANNEL_TYPE_UINT,//VK_FORMAT_R64_UINT
		VK_SHELL_IMAGE_CHANNEL_TYPE_UINT,//VK_FORMAT_R64G64_UINT
		VK_SHELL_IMAGE_CHANNEL_TYPE_UINT,//VK_FORMAT_R64G64B64_UINT
		VK_SHELL_IMAGE_CHANNEL_TYPE_UINT,//VK_FORMAT_R64G64B64A64_UINT
	
		VK_SHELL_IMAGE_CHANNEL_TYPE_SINT,//VK_FORMAT_R64_SINT
		VK_SHELL_IMAGE_CHANNEL_TYPE_SINT,//VK_FORMAT_R64G64_SINT
		VK_SHELL_IMAGE_CHANNEL_TYPE_SINT,//VK_FORMAT_R64G64B64_SINT
		VK_SHELL_IMAGE_CHANNEL_TYPE_SINT,//VK_FORMAT_R64G64B64A64_SINT
	
		VK_SHELL_IMAGE_CHANNEL_TYPE_SFLOAT,//VK_FORMAT_R64_SFLOAT
		VK_SHELL_IMAGE_CHANNEL_TYPE_SFLOAT,//VK_FORMAT_R64G64_SFLOAT
		VK_SHELL_IMAGE_CHANNEL_TYPE_SFLOAT,//VK_FORMAT_R64G64B64_SFLOAT
		VK_SHELL_IMAGE_CHANNEL_TYPE_SFLOAT,//VK_FORMAT_R64G64B64A64_SFLOAT
	};

	uint32_t supportedFormatCount = 0;
	VkFormat rSupportedFormats [VK_SHELL_MAX_STACK_DEVICE_COLOR_FORMATS_QUERIES] = { 0 };
	uint32_t rChannelsCount    [VK_SHELL_MAX_STACK_DEVICE_COLOR_FORMATS_QUERIES] = { 0 };
	uint32_t rChannelsSizes    [VK_SHELL_MAX_STACK_DEVICE_COLOR_FORMATS_QUERIES] = { 0 };
	uint32_t rChannelsTypes    [VK_SHELL_MAX_STACK_DEVICE_COLOR_FORMATS_QUERIES] = { 0 };

	for (uint32_t formatIdx = 0; formatIdx < VK_SHELL_MAX_STACK_DEVICE_COLOR_FORMATS_QUERIES; formatIdx++) {
		
		VkFormat format         = colorFormats [formatIdx];
		uint32_t channelCount  = channelsCount[formatIdx];
		uint32_t channelSize   = channelsSizes[formatIdx];
		uint32_t channelType  = channelsTypes[formatIdx];

		uint8_t  supported     = 0;
		
		VkShellResult formatResult = vkShellCheckSupportedDeviceColorFormat(physicalDevice, format, &supported);
		if (formatResult != VK_SHELL_SUCCESS) {
			return formatResult;
		}

		if (
			(supported) && 
			(channelCount >= minChannelCount && channelCount <= maxChannelCount) &&
			(channelSize >= minChannelSize && channelSize <= maxChannelSize) &&
			(channelTypes & channelType)
		) {
			rSupportedFormats     [supportedFormatCount] = format;
			rChannelsCount        [supportedFormatCount] = channelCount;
			rChannelsSizes        [supportedFormatCount] = channelSize;
			rChannelsTypes        [supportedFormatCount] = channelType;
			supportedFormatCount++;
		}
	}

	(*pSupportedFormatCount) = supportedFormatCount;
	memcpy(pSupportedFormats,     rSupportedFormats, supportedFormatCount * sizeof(VkFormat));
	memcpy(pChannelsCount,        rChannelsCount,    supportedFormatCount * sizeof(uint32_t));
	memcpy(pSingleChannelsSizes, rChannelsSizes,    supportedFormatCount * sizeof(uint32_t));
	memcpy(pChannelsTypes,        rChannelsTypes,    supportedFormatCount * sizeof(uint32_t));

	return VK_SHELL_SUCCESS;
}

VkResult vkShellCreateSwapchain(VkShellCreateSwapchainArgs args) {
	if (
		args.device == VK_NULL_HANDLE ||
		args.physicalDevice == VK_NULL_HANDLE ||
		args.surface == VK_NULL_HANDLE ||
		args.swapchainImageCount == 0 ||
		args.pSwapchainImageCount == VK_NULL_HANDLE ||
		args.pSwapchain == VK_NULL_HANDLE
	) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}
	if (args.imageSharingMode == VK_SHARING_MODE_CONCURRENT && (args.queueFamilyIndexCount < 2 || args.pQueueFamilyIndices == VK_NULL_HANDLE)) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}
	if ((args.surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) == 0) {
		return VK_ERROR_FORMAT_NOT_SUPPORTED;
	}

	uint32_t formatCount = 0;
	VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(args.physicalDevice, args.surface, &formatCount, VK_NULL_HANDLE);
	if (result != VK_SUCCESS) {
		return result;
	}
	if (formatCount == 0) {
		return VK_ERROR_FORMAT_NOT_SUPPORTED;
	}
	if (formatCount > VK_SHELL_MAX_STACK_DEVICE_SURFACE_FORMAT_COUNT) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkSurfaceFormatKHR surfaceFormats[VK_SHELL_MAX_STACK_DEVICE_SURFACE_FORMAT_COUNT] = { 0 };
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(args.physicalDevice, args.surface, &formatCount, surfaceFormats);
	if (result != VK_SUCCESS) {
		return result;
	}

	VkSurfaceFormatKHR surfaceFormat = surfaceFormats[0];
	if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
		surfaceFormat.format = args.imageFormat;
	}
	else {
		for (uint32_t i = 0; i < formatCount; i++) {
			if (surfaceFormats[i].format == args.imageFormat) {
				surfaceFormat = surfaceFormats[i];
				break;
			}
		}
	}
	if (surfaceFormat.format == VK_FORMAT_UNDEFINED) {
		return VK_ERROR_FORMAT_NOT_SUPPORTED;
	}

	uint32_t swapchainImageCount = args.swapchainImageCount;
	if (swapchainImageCount < args.surfaceCapabilities.minImageCount) {
		swapchainImageCount = args.surfaceCapabilities.minImageCount;
	}
	if (args.surfaceCapabilities.maxImageCount != 0 && swapchainImageCount > args.surfaceCapabilities.maxImageCount) {
		swapchainImageCount = args.surfaceCapabilities.maxImageCount;
	}

	VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[] = {
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
	};
	VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	for (uint32_t i = 0; i < sizeof(compositeAlphaFlags) / sizeof(compositeAlphaFlags[0]); i++) {
		if (args.surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
			compositeAlpha = compositeAlphaFlags[i];
			break;
		}
	}

	uint32_t presentModeCount = 0;
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(args.physicalDevice, args.surface, &presentModeCount, VK_NULL_HANDLE);
	if (result != VK_SUCCESS) {
		return result;
	}
	if (presentModeCount > VK_SHELL_MAX_STACK_SURFACE_PRESENT_MODE_COUNT) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkPresentModeKHR presentModes[VK_SHELL_MAX_STACK_SURFACE_PRESENT_MODE_COUNT] = { 0 };
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(args.physicalDevice, args.surface, &presentModeCount, presentModes);
	if (result != VK_SUCCESS) {
		return result;
	}

	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	if (presentModeCount == 0) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}
	if (!args.vsync) {
		for (uint32_t i = 0; i < presentModeCount; i++) {
			if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
				presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				break;
			}
		}
	}
	if (args.surfaceCapabilities.currentExtent.width == 0 || args.surfaceCapabilities.currentExtent.height == 0) {
		return VK_ERROR_OUT_OF_DATE_KHR;
	}

	VkSwapchainCreateInfoKHR swapchainCreateInfo = {
		.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext                 = VK_NULL_HANDLE,
		.flags                 = 0,
		.surface               = args.surface,
		.minImageCount         = swapchainImageCount,
		.imageFormat           = surfaceFormat.format,
		.imageColorSpace       = surfaceFormat.colorSpace,
		.imageExtent           = args.surfaceCapabilities.currentExtent,
		.imageArrayLayers      = 1,
		.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode      = args.imageSharingMode,
		.queueFamilyIndexCount = args.queueFamilyIndexCount,
		.pQueueFamilyIndices   = args.pQueueFamilyIndices,
		.preTransform          = args.surfaceCapabilities.currentTransform,
		.compositeAlpha        = compositeAlpha,
		.presentMode           = presentMode,
		.clipped               = VK_TRUE,
		.oldSwapchain          = args.oldSwapchain
	};

	result = vkCreateSwapchainKHR(args.device, &swapchainCreateInfo, VK_NULL_HANDLE, args.pSwapchain);
	if (result != VK_SUCCESS) {
		return result;
	}

	*args.pSwapchainImageCount = swapchainImageCount;
	if (args.pImageFormat != VK_NULL_HANDLE) {
		*args.pImageFormat = surfaceFormat.format;
	}
	return VK_SUCCESS;
}

VkShellResult vkShellCombineMaxSamples(
	VkPhysicalDeviceProperties physicalDeviceProperties,
	uint32_t sampleCount,
	uint8_t combineColorSample,
	uint8_t combineDepthSample,
	uint32_t* pSampleCount
) {
	if (pSampleCount == VK_NULL_HANDLE || sampleCount == 0) {
		return VK_SHELL_ERROR_INVALID_ARGUMENT;
	}

	VkSampleCountFlags combinedSampleCount = 0;
	for (uint8_t i = 0; i < 32; i++) {
		combinedSampleCount |= (1u << i);
		if (sampleCount & (1u << i)) {
			break;
		}
	}

	if (combineColorSample) {
		combinedSampleCount &= physicalDeviceProperties.limits.framebufferColorSampleCounts;
	}
	if (combineDepthSample) {
		combinedSampleCount &= physicalDeviceProperties.limits.framebufferDepthSampleCounts;
	}

	if (combinedSampleCount & VK_SAMPLE_COUNT_64_BIT) { *pSampleCount = VK_SAMPLE_COUNT_64_BIT; return VK_SHELL_SUCCESS; }
	if (combinedSampleCount & VK_SAMPLE_COUNT_32_BIT) { *pSampleCount = VK_SAMPLE_COUNT_32_BIT; return VK_SHELL_SUCCESS; }
	if (combinedSampleCount & VK_SAMPLE_COUNT_16_BIT) { *pSampleCount = VK_SAMPLE_COUNT_16_BIT; return VK_SHELL_SUCCESS; }
	if (combinedSampleCount & VK_SAMPLE_COUNT_8_BIT)  { *pSampleCount = VK_SAMPLE_COUNT_8_BIT;  return VK_SHELL_SUCCESS; }
	if (combinedSampleCount & VK_SAMPLE_COUNT_4_BIT)  { *pSampleCount = VK_SAMPLE_COUNT_4_BIT;  return VK_SHELL_SUCCESS; }
	if (combinedSampleCount & VK_SAMPLE_COUNT_2_BIT)  { *pSampleCount = VK_SAMPLE_COUNT_2_BIT;  return VK_SHELL_SUCCESS; }
	if (combinedSampleCount & VK_SAMPLE_COUNT_1_BIT)  { *pSampleCount = VK_SAMPLE_COUNT_1_BIT;  return VK_SHELL_SUCCESS; }

	return VK_SHELL_ERROR_UNSUPPORTED;
}

VkResult vkShellDestroyFramebuffers(VkDevice device, uint32_t framebufferCount, VkFramebuffer* pFramebuffers) {
	if (device == VK_NULL_HANDLE || framebufferCount == 0 || pFramebuffers == VK_NULL_HANDLE) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}
	VkResult result = vkDeviceWaitIdle(device);
	if (result != VK_SUCCESS) {
		return result;
	}
	for (uint32_t i = 0; i < framebufferCount; i++) {
		vkDestroyFramebuffer(device, pFramebuffers[i], VK_NULL_HANDLE);
	}
	return VK_SUCCESS;
}

VkResult vkShellQueueSubmit(
	uint32_t cmdBufferCount,
	VkCommandBuffer* pCmdBuffers,
	VkQueue queue,
	VkFence fence,
	uint32_t semaphoresToWaitForCount,
	VkSemaphore* pSemaphoresToWaitFor,
	VkPipelineStageFlags waitStage,
	uint32_t signalSemaphoreCount,
	VkSemaphore* pSignalSemaphores
) {
	if (cmdBufferCount == 0 || pCmdBuffers == VK_NULL_HANDLE || queue == VK_NULL_HANDLE) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}
	if (semaphoresToWaitForCount > 0 && pSemaphoresToWaitFor == VK_NULL_HANDLE) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}
	if (signalSemaphoreCount > 0 && pSignalSemaphores == VK_NULL_HANDLE) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkSubmitInfo submitInfo = {
		.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext                = VK_NULL_HANDLE,
		.waitSemaphoreCount   = semaphoresToWaitForCount,
		.pWaitSemaphores      = pSemaphoresToWaitFor,
		.pWaitDstStageMask    = semaphoresToWaitForCount > 0 ? &waitStage : VK_NULL_HANDLE,
		.commandBufferCount   = cmdBufferCount,
		.pCommandBuffers      = pCmdBuffers,
		.signalSemaphoreCount = signalSemaphoreCount,
		.pSignalSemaphores    = pSignalSemaphores
	};

	VkResult result = vkQueueSubmit(queue, 1, &submitInfo, fence);
	if (result != VK_SUCCESS) {
		return result;
	}
	return VK_SUCCESS;
}

VkResult vkShellWaitForQueue(VkQueue queue) {
	if (queue == VK_NULL_HANDLE) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}
	VkResult result = vkQueueWaitIdle(queue);
	if (result != VK_SUCCESS) {
		return result;
	}
	return VK_SUCCESS;
}

VkResult vkShellCreateFences(VkDevice device, uint32_t fenceCount, uint8_t signaled, VkFence* pFences) {
	if (device == VK_NULL_HANDLE || fenceCount == 0 || pFences == VK_NULL_HANDLE) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkFenceCreateInfo fenceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0
	};

	for (uint32_t i = 0; i < fenceCount; i++) {
		VkResult result = vkCreateFence(device, &fenceCreateInfo, VK_NULL_HANDLE, &pFences[i]);
		if (result != VK_SUCCESS) {
			for (uint32_t j = 0; j < i; j++) {
				vkDestroyFence(device, pFences[j], VK_NULL_HANDLE);
				pFences[j] = VK_NULL_HANDLE;
			}
			return result;
		}
	}
	return VK_SUCCESS;
}

VkResult vkShellCreateSemaphores(VkDevice device, uint32_t semaphoreCount, VkSemaphore* pSemaphores) {
	if (device == VK_NULL_HANDLE || semaphoreCount == 0 || pSemaphores == VK_NULL_HANDLE) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkSemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = 0
	};

	for (uint32_t i = 0; i < semaphoreCount; i++) {
		VkResult result = vkCreateSemaphore(device, &semaphoreCreateInfo, VK_NULL_HANDLE, &pSemaphores[i]);
		if (result != VK_SUCCESS) {
			for (uint32_t j = 0; j < i; j++) {
				vkDestroySemaphore(device, pSemaphores[j], VK_NULL_HANDLE);
				pSemaphores[j] = VK_NULL_HANDLE;
			}
			return result;
		}
	}
	return VK_SUCCESS;
}

VkShellResult vkShellGetBufferMemoryLayout(VkShellGetBufferMemoryLayoutArgs args) {
	if (args.pMemory == NULL || args.bufferCount == 0 || args.pBuffers == NULL) {
		return VK_SHELL_ERROR_INVALID_ARGUMENT;
	}

	VkDeviceSize allocationSize = 0;
	uint32_t memoryTypeBits = UINT32_MAX;

	for (uint32_t i = 0; i < args.bufferCount; i++) {
		VkShellBufferData* pBuffer = &args.pBuffers[i];
		VkDeviceSize alignment = pBuffer->memoryRequirements.alignment;

		if (pBuffer->buffer == VK_NULL_HANDLE || alignment == 0) {
			return VK_SHELL_ERROR_INVALID_ARGUMENT;
		}

		VkDeviceSize remainder = allocationSize % alignment;
		VkDeviceSize padding = remainder == 0 ? 0 : alignment - remainder;

		if (allocationSize > UINT64_MAX - padding) {
			return VK_SHELL_ERROR_LIMIT_EXCEEDED;
		}

		pBuffer->memoryOffset = allocationSize + padding;

		if (pBuffer->memoryRequirements.size > UINT64_MAX - pBuffer->memoryOffset) {
			return VK_SHELL_ERROR_LIMIT_EXCEEDED;
		}

		allocationSize = pBuffer->memoryOffset + pBuffer->memoryRequirements.size;
		memoryTypeBits &= pBuffer->memoryRequirements.memoryTypeBits;
	}

	args.pMemory->allocationSize = allocationSize;
	args.pMemory->memoryTypeBits = memoryTypeBits;
	args.pMemory->bufferCount = args.bufferCount;
	args.pMemory->pBuffers = args.pBuffers;

	return VK_SHELL_SUCCESS;
}

VkShellResult vkShellGetMemoryTypeIndex(
	VkDevice device,
	VkPhysicalDevice physicalDevice,
	uint32_t memoryTypeBits,
	VkMemoryPropertyFlags propertyFlags,
	uint32_t* pMemoryTypeIndex
) {
	if (device == VK_NULL_HANDLE || physicalDevice == VK_NULL_HANDLE || pMemoryTypeIndex == VK_NULL_HANDLE) {
		return VK_SHELL_ERROR_INVALID_ARGUMENT;
	}

	VkPhysicalDeviceMemoryProperties memoryProperties = { 0 };
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		if ((memoryTypeBits & (1u << i)) && (memoryProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags) {
			*pMemoryTypeIndex = i;
			return VK_SHELL_SUCCESS;
		}
	}

	return VK_SHELL_ERROR_NOT_FOUND;
}

VkShellResult vkShellGetMemoryBudgetProperties(
	VkPhysicalDevice physicalDevice,
	VkPhysicalDeviceMemoryBudgetPropertiesEXT* pMemoryBudgetProperties
) {
	if (physicalDevice == VK_NULL_HANDLE || pMemoryBudgetProperties == VK_NULL_HANDLE) {
		return VK_SHELL_ERROR_INVALID_ARGUMENT;
	}

	VkPhysicalDeviceMemoryBudgetPropertiesEXT memoryBudgetProperties = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT,
		.pNext = VK_NULL_HANDLE
	};
	VkPhysicalDeviceMemoryProperties2 memoryProperties2 = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2,
		.pNext = &memoryBudgetProperties,
	};
	vkGetPhysicalDeviceMemoryProperties2(physicalDevice, &memoryProperties2);

	*pMemoryBudgetProperties = memoryBudgetProperties;
	return VK_SHELL_SUCCESS;
}



#ifdef __cplusplus
}
#endif//__cplusplus
