#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include <vk-shell/vk-shell.h>
#include <stdio.h>



typedef enum VkToolkitLogLevel {
	VK_TOOLKIT_LOG_LEVEL_DEBUG,
	VK_TOOLKIT_LOG_LEVEL_STATUS,
	VK_TOOLKIT_LOG_LEVEL_WARNING,
	VK_TOOLKIT_LOG_LEVEL_ERROR
} VkToolkitLogLevel;

static void vkToolkitLog(
	uint8_t condition,
	const char* appName,
	VkToolkitLogLevel logLevel,
	const char* message,
	const char* file,
	const char* func,
	int line
) {
	if (!condition) {
		return;
	}

	const char* logLevelName = "UNKNOWN";
	switch (logLevel) {
		case VK_TOOLKIT_LOG_LEVEL_DEBUG:
			logLevelName = "DEBUG";
			break;
		case VK_TOOLKIT_LOG_LEVEL_STATUS:
			logLevelName = "STATUS";
			break;
		case VK_TOOLKIT_LOG_LEVEL_WARNING:
			logLevelName = "WARNING";
			break;
		case VK_TOOLKIT_LOG_LEVEL_ERROR:
			logLevelName = "ERROR";
			break;
	}

	printf(
		"[vkToolkit] @ [%s] @ %s:%s:%i | %s | %s\n",
		appName != NULL ? appName : "vk-toolkit",
		file,
		func,
		line,
		logLevelName,
		message != NULL ? message : ""
	);
}

#define VK_TOOLKIT_LOG(condition, appName, logLevel, message)\
	do {\
		vkToolkitLog(condition, appName, logLevel, message, __FILE__, __func__, __LINE__);\
	} while (0)

#ifndef VK_TOOLKIT_APP_NAME
	#define VK_TOOLKIT_APP_NAME "vk-toolkit"
#endif

#define VK_TOOLKIT_ERROR(condition, errorMsg, failureExpression)\
	do {\
		if ((condition)) {\
			VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_ERROR, (errorMsg));\
			failureExpression;\
		}\
	} while (0)

#define VK_TOOLKIT_VK_RESULT_ERROR(result, failureExpression)\
	do {\
		const VkResult vkToolkitVkResult = (VkResult)(result);\
		if (vkToolkitVkResult != VK_SUCCESS) {\
			VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_ERROR, vkShellTranslateVkResult(vkToolkitVkResult));\
			failureExpression;\
		}\
	} while (0)

#define VK_TOOLKIT_SHELL_RESULT_ERROR(result, failureExpression)\
	do {\
		const VkShellResult vkToolkitShellResult = (VkShellResult)(result);\
		if (vkToolkitShellResult != VK_SHELL_SUCCESS) {\
			VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_ERROR, vkShellTranslateResult(vkToolkitShellResult));\
			failureExpression;\
		}\
	} while (0)

#define VK_TOOLKIT_SHELL_CHECK(functionCall, failureExpression)\
	VK_TOOLKIT_SHELL_RESULT_ERROR((functionCall), failureExpression)



#ifdef GRAPHICS_EXAMPLE
	#define GLFW_INCLUDE_NONE
	#define GLFW_INCLUDE_VULKAN
	#include <GLFW/glfw3.h>
#endif//GRAPHICS_EXAMPLE

#ifdef GRAPHICS_EXAMPLE
typedef struct Commands {
	VkCommandPool graphicsCmdPool;
	VkCommandBuffer graphicsCmdBuffers[MAX_FRAMES_IN_FLIGHT];
	VkFence         graphicsCmdFences[MAX_FRAMES_IN_FLIGHT];
} Commands;
#endif//GRAPHICS_EXAMPLE

static VkResult createBuffer(
	VkDevice device,
	VkDeviceSize size,
	VkBufferUsageFlags usage,
	VkShellBufferData* pBuffer
) {
	if (pBuffer == NULL) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkBufferCreateInfo createInfo = {
		.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext                 = NULL,
		.flags                 = 0,
		.size                  = size,
		.usage                 = usage,
		.sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices   = NULL
	};

	VkResult result = vkCreateBuffer(device, &createInfo, NULL, &pBuffer->buffer);
	if (result != VK_SUCCESS) {
		return result;
	}

	pBuffer->size = size;
	vkGetBufferMemoryRequirements(device, pBuffer->buffer, &pBuffer->memoryRequirements);

	return VK_SUCCESS;
}

static VkResult createImageView2D(
	VkDevice device,
	VkImage image,
	VkFormat format,
	VkImageAspectFlags aspectMask,
	VkImageView* pImageView
) {
	if (pImageView == NULL) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkImageViewCreateInfo createInfo = {
		.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext    = NULL,
		.flags    = 0,
		.image    = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format   = format,
		.components = { VK_COMPONENT_SWIZZLE_IDENTITY },
		.subresourceRange = {
			.aspectMask     = aspectMask,
			.baseMipLevel   = 0,
			.levelCount     = 1,
			.baseArrayLayer = 0,
			.layerCount     = 1
		}
	};

	return vkCreateImageView(device, &createInfo, NULL, pImageView);
}




#ifdef GRAPHICS_EXAMPLE

typedef struct SurfaceData {
	VkSurfaceKHR             surface;
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
} SurfaceData;

typedef struct SwapchainData {
	VkSwapchainKHR swapchain;

	VkExtent2D extent;

	VkFormat swapchainImageFormat;
	uint32_t swapchainImageCount;

	VkImage     swapchainImages[MAX_STACK_SWAPCHAIN_IMAGE_COUNT];
	VkImageView swapchainImageViews[MAX_STACK_SWAPCHAIN_IMAGE_COUNT];

	VkSharingMode swapchainImageSharingMode;

} SwapchainData;

typedef struct FrameData {
	uint32_t framesInFlight;
} FrameData;

typedef struct RecreateSwapchainArgs {
	int            width;
	int            height;
	uint8_t        vsync;
	Driver*        pDriver;
	Queues*        pQueues;
	SurfaceData*   pSurfaceData;
	SwapchainData* pSwapchainData;
} RecreateSwapchainArgs;



static VkResult getSwapchainImages(VkDevice device, SwapchainData* pSwapchainData) {
	VkResult result = vkGetSwapchainImagesKHR(device, pSwapchainData->swapchain, &pSwapchainData->swapchainImageCount, NULL);
	VK_TOOLKIT_VK_RESULT_ERROR(result, return result);

	if (pSwapchainData->swapchainImageCount > MAX_STACK_SWAPCHAIN_IMAGE_COUNT) {
		VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_ERROR, "swapchain image count exceeds the example limit");
		return VK_ERROR_TOO_MANY_OBJECTS;
	}

	result = vkGetSwapchainImagesKHR(device, pSwapchainData->swapchain, &pSwapchainData->swapchainImageCount, pSwapchainData->swapchainImages);
	VK_TOOLKIT_VK_RESULT_ERROR(result, return result);

	return VK_SUCCESS;
}

static VkResult createSwapchainImageViews(VkDevice device, SwapchainData* pSwapchainData) {
	for (uint32_t i = 0; i < pSwapchainData->swapchainImageCount; i++) {
		VkResult result = createImageView2D(
			device,
			pSwapchainData->swapchainImages[i],
			pSwapchainData->swapchainImageFormat,
			VK_IMAGE_ASPECT_COLOR_BIT,
			&pSwapchainData->swapchainImageViews[i]
		);
		if (result != VK_SUCCESS) {
			for (uint32_t j = 0; j < i; j++) {
				vkDestroyImageView(device, pSwapchainData->swapchainImageViews[j], NULL);
				pSwapchainData->swapchainImageViews[j] = VK_NULL_HANDLE;
			}
			VK_TOOLKIT_VK_RESULT_ERROR(result, return result);
		}
	}

	return VK_SUCCESS;
}

static VkResult createSwapchain(
	int width,
	int height,
	VkFormat imageFormat,
	uint8_t vsync,
	VkSwapchainKHR oldSwapchain,
	Driver* pDriver,
	Queues* pQueues,
	SurfaceData* pSurfaceData,
	SwapchainData* pSwapchainData
) {
	VkResult result = vkShellGetPhysicalDeviceSurfaceCapabilities(
		pDriver->physicalDevice,
		pSurfaceData->surface,
		&pSurfaceData->surfaceCapabilities,
		width,
		height
	);
	if (result != VK_SUCCESS) {
		return result;
	}

	pSwapchainData->swapchainImageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (pQueues->graphicsQueueFamilyIndex != pQueues->presentQueueFamilyIndex) {
		pSwapchainData->swapchainImageSharingMode = VK_SHARING_MODE_CONCURRENT;
	}
	uint32_t queueFamilyIndices[2] = {
		pQueues->graphicsQueueFamilyIndex,
		pQueues->presentQueueFamilyIndex
	};

	result = vkShellCreateSwapchain((VkShellCreateSwapchainArgs) {
		.device                  = pDriver->device,
		.physicalDevice         = pDriver->physicalDevice,
		.surfaceCapabilities    = pSurfaceData->surfaceCapabilities,
		.surface                 = pSurfaceData->surface,
		.imageFormat             = imageFormat,
		.pImageFormat            = &pSwapchainData->swapchainImageFormat,
		.swapchainImageCount    = SWAPCHAIN_IMAGE_COUNT,
		.imageSharingMode       = pSwapchainData->swapchainImageSharingMode,
		.queueFamilyIndexCount  = pSwapchainData->swapchainImageSharingMode == VK_SHARING_MODE_CONCURRENT ? 2u : 0u,
		.pQueueFamilyIndices    = pSwapchainData->swapchainImageSharingMode == VK_SHARING_MODE_CONCURRENT ? queueFamilyIndices : VK_NULL_HANDLE,
		.vsync                  = vsync,
		.pSwapchainImageCount   = &pSwapchainData->swapchainImageCount,
		.oldSwapchain           = oldSwapchain,
		.pSwapchain             = &pSwapchainData->swapchain
	});
	if (result != VK_SUCCESS) {
		return result;
	}

	pSwapchainData->extent = pSurfaceData->surfaceCapabilities.currentExtent;
	return VK_SUCCESS;
}

static VkResult vkToolkitRecreateSwapchain(RecreateSwapchainArgs args) {
	VkResult result = vkDeviceWaitIdle(args.pDriver->device);
	VK_TOOLKIT_VK_RESULT_ERROR(result, return result);

	VkSwapchainKHR oldSwapchain = args.pSwapchainData->swapchain;
	uint32_t oldSwapchainImageCount = args.pSwapchainData->swapchainImageCount;

	result = createSwapchain(
		args.width,
		args.height,
		args.pSwapchainData->swapchainImageFormat,
		args.vsync,
		oldSwapchain,
		args.pDriver,
		args.pQueues,
		args.pSurfaceData,
		args.pSwapchainData
	);
	if (result != VK_SUCCESS) {
		return result;
	}

	for (uint32_t i = 0; i < oldSwapchainImageCount; i++) {
		vkDestroyImageView(args.pDriver->device, args.pSwapchainData->swapchainImageViews[i], NULL);
	}
	vkDestroySwapchainKHR(args.pDriver->device, oldSwapchain, NULL);

	result = getSwapchainImages(args.pDriver->device, args.pSwapchainData);
	if (result != VK_SUCCESS) {
		return result;
	}

	return createSwapchainImageViews(args.pDriver->device, args.pSwapchainData);
}



static GLFWwindow* initializeWindow(const char* title, int* pWidth, int* pHeight) {
	VK_TOOLKIT_ERROR(glfwInit() == GLFW_FALSE, "failed initializing glfw", return NULL);
	VK_TOOLKIT_ERROR(glfwVulkanSupported() == GLFW_FALSE, "glfw does not support vulkan", return NULL;);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE,  GLFW_TRUE);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	VK_TOOLKIT_ERROR(monitor == NULL, "failed getting the primary monitor", return NULL);

	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
	VK_TOOLKIT_ERROR(videoMode == NULL, "failed getting the primary monitor video mode", return NULL);

	(*pWidth)  = (int)((float)videoMode->width  / 1.5f);
	(*pHeight) = (int)((float)videoMode->height / 1.5f);

	GLFWwindow* window = glfwCreateWindow((*pWidth), (*pHeight), title, NULL, NULL);
	VK_TOOLKIT_ERROR(window == NULL, "failed creating the window", return NULL);

	glfwSetWindowSizeLimits(window, 400, 300, GLFW_DONT_CARE, GLFW_DONT_CARE);

	return window;
}

static VkResult initializeVulkan(
	GLFWwindow* window,
	uint32_t apiVersion,
	Driver* pDriver,
	Queues* pQueues,
	SurfaceData* pSurfaceData
) {
	uint32_t     instanceExtensionCount = 0;
	const char** ppInstanceExtensions   = glfwGetRequiredInstanceExtensions(&instanceExtensionCount);

	VkResult result = vkShellCreateInstance((VkShellCreateInstanceArgs) {
		.applicationName         = "vulkan app",
		.engineName              = "vulkan engine",
		#ifdef DEBUG
		.enableValidationLayers = 1,
		#else
		.enableValidationLayers = 0,
		#endif//DEBUG
		.extensionCount          = instanceExtensionCount,
		.ppExtensionNames       = ppInstanceExtensions,
		.apiVersion              = apiVersion,
		.pInstance               = &pDriver->instance
	});
	if (result != VK_SUCCESS) {
		return result;
	}

	result = glfwCreateWindowSurface(pDriver->instance, window, VK_NULL_HANDLE, &pSurfaceData->surface);
	VK_TOOLKIT_VK_RESULT_ERROR(result, return result);

	VkShellResult shellResult = vkShellSelectPhysicalDevice((VkShellSelectPhysicalDeviceArgs) {
		.instance                        = pDriver->instance,
		.surface                         = pSurfaceData->surface,
		.queuesRequirements              = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT,
		.dynamicRendering                = 1,
		.synchronization2                = 1,
		.pPhysicalDevice           = &pDriver->physicalDevice,
		.pPhysicalDeviceProperties = &pDriver->physicalDeviceProperties
	});
	if (shellResult != VK_SHELL_SUCCESS) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	uint32_t graphicsQueueFamiliesIndices[VK_SHELL_MAX_STACK_QUEUE_FAMILY_COUNT] = { 0 };
	uint32_t presentQueueFamiliesIndices [VK_SHELL_MAX_STACK_QUEUE_FAMILY_COUNT] = { 0 };

	shellResult = vkShellGetPhysicalDeviceQueueFamilies((VkShellGetPhysicalDeviceQueueFamiliesArgs) {
		.physicalDevice                 = pDriver->physicalDevice,
		.surface                         = pSurfaceData->surface,
		.pQueueFamilyCount            = VK_NULL_HANDLE,
		.pGraphicsQueueFamilyCount   = VK_NULL_HANDLE,
		.pSurfaceQueueFamilyCount    = VK_NULL_HANDLE,
		.pComputeQueueFamilyCount    = VK_NULL_HANDLE,
		.pTransferQueueFamilyCount   = VK_NULL_HANDLE,
		.pGraphicsQueueFamilyIndices = graphicsQueueFamiliesIndices,
		.pSurfaceQueueFamilyIndices  = presentQueueFamiliesIndices,
		.pComputeQueueFamilyIndices  = VK_NULL_HANDLE,
		.pTransferQueueFamilyIndices = VK_NULL_HANDLE,
		.pQueueFamiliesProperties     = VK_NULL_HANDLE
	});
	if (shellResult != VK_SHELL_SUCCESS) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	pQueues->graphicsQueueFamilyIndex = graphicsQueueFamiliesIndices[0];
	pQueues->presentQueueFamilyIndex  = presentQueueFamiliesIndices [0];

	float defaultQueuePriority = 1.0f;

	VkDeviceQueueCreateInfo graphicsDeviceQueueInfo = {
		.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext            = NULL,
		.flags            = 0,
		.queueFamilyIndex = pQueues->graphicsQueueFamilyIndex,
		.queueCount       = 1,
		.pQueuePriorities = &defaultQueuePriority
	};

	VkDeviceQueueCreateInfo presentDeviceQueueInfo = {
		.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext            = NULL,
		.flags            = 0,
		.queueFamilyIndex = pQueues->presentQueueFamilyIndex,
		.queueCount       = 1,
		.pQueuePriorities = &defaultQueuePriority
	};

	VkDeviceQueueCreateInfo deviceQueueInfos[2] = {
		graphicsDeviceQueueInfo,
		presentDeviceQueueInfo
	};
	const char* deviceExtensions[1] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	uint32_t deviceQueueCount = (pQueues->graphicsQueueFamilyIndex == pQueues->presentQueueFamilyIndex) ? 1 : 2;

	result = vkShellSetLogicalDevice((VkShellSetLogicalDeviceArgs) {
		.physicalDevice      = pDriver->physicalDevice,
		.pDevice             = &pDriver->device,
		.extensionCount      = 1,
		.ppExtensionNames   = deviceExtensions,
		.deviceQueueCount   = deviceQueueCount,
		.pDeviceQueueInfos = deviceQueueInfos
	});
	if (result != VK_SUCCESS) {
		return result;
	}

	vkGetDeviceQueue(pDriver->device, pQueues->graphicsQueueFamilyIndex, 0, &pQueues->graphicsQueue);
	vkGetDeviceQueue(pDriver->device, pQueues->presentQueueFamilyIndex,  0, &pQueues->presentQueue);

	return VK_SUCCESS;
}

static VkResult createSwapchainResources(
	Driver* pDriver,
	Queues* pQueues,
	Commands* pCommands,
	SwapchainData* pSwapchainData,
	FrameData* pFrameData
) {
	pFrameData->framesInFlight = pSwapchainData->swapchainImageCount < MAX_FRAMES_IN_FLIGHT ? pSwapchainData->swapchainImageCount : MAX_FRAMES_IN_FLIGHT;

	VkCommandPoolCreateInfo commandPoolCreateInfo = {
		.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = pQueues->graphicsQueueFamilyIndex
	};
	VkResult result = vkCreateCommandPool(pDriver->device, &commandPoolCreateInfo, NULL, &pCommands->graphicsCmdPool);
	VK_TOOLKIT_VK_RESULT_ERROR(result, return result);

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
		.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool        = pCommands->graphicsCmdPool,
		.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = pFrameData->framesInFlight
	};
	result = vkAllocateCommandBuffers(pDriver->device, &commandBufferAllocateInfo, pCommands->graphicsCmdBuffers);
	VK_TOOLKIT_VK_RESULT_ERROR(result, return result);

	result = vkShellCreateFences(pDriver->device, pFrameData->framesInFlight, 1, pCommands->graphicsCmdFences);
	if (result != VK_SUCCESS) {
		return result;
	}

	result = getSwapchainImages(pDriver->device, pSwapchainData);
	if (result != VK_SUCCESS) {
		return result;
	}
	result = createSwapchainImageViews(pDriver->device, pSwapchainData);
	if (result != VK_SUCCESS) {
		return result;
	}

	result = vkShellCreateSemaphores(pDriver->device, pSwapchainData->swapchainImageCount, pQueues->graphicsQueueFinishedSemaphores);
	if (result != VK_SUCCESS) {
		return result;
	}

	return vkShellCreateSemaphores(pDriver->device, pFrameData->framesInFlight, pQueues->imageAcquiredSemaphores);
}



static void endRendering(VkCommandBuffer cmdBuffer, VkImage image) {
	vkCmdEndRendering(cmdBuffer);

	VkImageMemoryBarrier2 toPresentationMemoryBarrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,

		.srcStageMask  = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,

		.dstStageMask = VK_PIPELINE_STAGE_2_NONE,
		.dstAccessMask = VK_ACCESS_2_NONE,

		.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,

		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

		.image = image,

		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	VkDependencyInfo dependencyInfo = {
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &toPresentationMemoryBarrier
	};

	vkCmdPipelineBarrier2(cmdBuffer, &dependencyInfo);
}





static void setViewport(VkCommandBuffer cmdBuffer, VkExtent2D extent) {
	VkViewport viewport = {
		.x        = 0.0f,
		.y        = 0.0f,
		.width    = (float)extent.width,
		.height   = (float)extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	VkRect2D scissor = {
		.offset = (VkOffset2D) { 0, 0 },
		.extent = extent
	};

	vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
	vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
}

static VkResult submitGraphics(
	VkQueue graphicsQueue,
	VkCommandBuffer cmdBuffer,
	VkFence fence,
	VkSemaphore waitSemaphore,
	VkSemaphore signalSemaphore
) {
	VkSemaphoreSubmitInfo waitSemaphoreInfo = {
		.sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		.pNext       = VK_NULL_HANDLE,
		.semaphore   = waitSemaphore,
		.value       = 0,
		.stageMask   = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		.deviceIndex = 0,
	};

	VkCommandBufferSubmitInfo commandBufferSubmitInfo = {
		.sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
		.pNext         = VK_NULL_HANDLE,
		.commandBuffer = cmdBuffer,
		.deviceMask    = 0
	};

	VkSemaphoreSubmitInfo signalSemaphoreInfo = {
		.sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		.pNext       = VK_NULL_HANDLE,
		.semaphore   = signalSemaphore,
		.value       = 0,
		.stageMask   = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		.deviceIndex = 0,
	};

	VkSubmitInfo2 submitInfo = {
		.sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
		.pNext                    = VK_NULL_HANDLE,
		.flags                    = 0,
		.waitSemaphoreInfoCount   = 1,
		.pWaitSemaphoreInfos      = &waitSemaphoreInfo,
		.commandBufferInfoCount   = 1,
		.pCommandBufferInfos      = &commandBufferSubmitInfo,
		.signalSemaphoreInfoCount = 1,
		.pSignalSemaphoreInfos    = &signalSemaphoreInfo
	};

	return vkQueueSubmit2(graphicsQueue, 1, &submitInfo, fence);
}

static VkResult presentImage(
	VkQueue presentQueue,
	VkSemaphore waitSemaphore,
	VkSwapchainKHR swapchain,
	uint32_t swapchainImageIdx,
	VkResult* pSwapchainResult
) {
	VkPresentInfoKHR presentInfo = {
		.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext              = VK_NULL_HANDLE,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores    = &waitSemaphore,
		.swapchainCount     = 1,
		.pSwapchains        = &swapchain,
		.pImageIndices      = &swapchainImageIdx,
		.pResults           = pSwapchainResult
	};

	return vkQueuePresentKHR(presentQueue, &presentInfo);
}

#endif//GRAPHICS_EXAMPLE

#ifdef __cplusplus
}
#endif//__cplusplus



#ifdef __cplusplus

#include <slang.h>
#include <slang-com-ptr.h>

#include <vector>

static const char* vkToolkitTranslateSlangResult(SlangResult result) {
	switch (result) {
		case SLANG_OK:
			return "SLANG_OK";
		case SLANG_FAIL:
			return "SLANG_FAIL";
		case SLANG_E_NOT_IMPLEMENTED:
			return "SLANG_E_NOT_IMPLEMENTED";
		case SLANG_E_NO_INTERFACE:
			return "SLANG_E_NO_INTERFACE";
		case SLANG_E_ABORT:
			return "SLANG_E_ABORT";
		case SLANG_E_INVALID_HANDLE:
			return "SLANG_E_INVALID_HANDLE";
		case SLANG_E_INVALID_ARG:
			return "SLANG_E_INVALID_ARG";
		case SLANG_E_OUT_OF_MEMORY:
			return "SLANG_E_OUT_OF_MEMORY";
		case SLANG_E_BUFFER_TOO_SMALL:
			return "SLANG_E_BUFFER_TOO_SMALL";
		case SLANG_E_UNINITIALIZED:
			return "SLANG_E_UNINITIALIZED";
		case SLANG_E_PENDING:
			return "SLANG_E_PENDING";
		case SLANG_E_CANNOT_OPEN:
			return "SLANG_E_CANNOT_OPEN";
		case SLANG_E_NOT_FOUND:
			return "SLANG_E_NOT_FOUND";
		case SLANG_E_INTERNAL_FAIL:
			return "SLANG_E_INTERNAL_FAIL";
		case SLANG_E_NOT_AVAILABLE:
			return "SLANG_E_NOT_AVAILABLE";
		case SLANG_E_TIME_OUT:
			return "SLANG_E_TIME_OUT";
		default:
			return "Unhandled Slang result";
	}
}

static void vkToolkitLogSlangDiagnostics(const Slang::ComPtr<slang::IBlob>& diagnostics) {
	if (diagnostics != NULL) {
		VK_TOOLKIT_LOG(
			1,
			VK_TOOLKIT_APP_NAME,
			VK_TOOLKIT_LOG_LEVEL_WARNING,
			(const char*)diagnostics->getBufferPointer()
		);
	}
}

#define VK_TOOLKIT_SLANG_RESULT_ERROR(result, failureExpression)\
	do {\
		const SlangResult vkToolkitSlangResult = (SlangResult)(result);\
		if (SLANG_FAILED(vkToolkitSlangResult)) {\
			VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_ERROR, vkToolkitTranslateSlangResult(vkToolkitSlangResult));\
			failureExpression;\
		}\
	} while (0)

#define VK_TOOLKIT_SLANG_CHECK(functionCall, failureExpression)\
	VK_TOOLKIT_SLANG_RESULT_ERROR((functionCall), failureExpression)

typedef struct VkToolkitSlangProgram {
	Slang::ComPtr<slang::IGlobalSession> globalSession;
	Slang::ComPtr<slang::ISession> session;
	Slang::ComPtr<slang::IBlob> diagnostics;
	Slang::ComPtr<slang::IModule> module;
	std::vector<Slang::ComPtr<slang::IEntryPoint>> entryPoints;
	Slang::ComPtr<slang::IComponentType> linkedProgram;
	slang::ProgramLayout* pLayout;
	std::vector<Slang::ComPtr<slang::IBlob>> entryPointCodes;
} VkToolkitSlangProgram;

typedef struct VkToolkitCompileSlangProgramArgs {
	const slang::SessionDesc* pSessionDesc;
	const char* modulePath;
	uint32_t entryPointCount;
	const char* const* ppEntryPointNames;
	SlangInt targetIndex;
	VkToolkitSlangProgram* pProgram;
} VkToolkitCompileSlangProgramArgs;

static SlangResult vkToolkitCompileSlangProgram(VkToolkitCompileSlangProgramArgs args) {
	if (args.pSessionDesc == NULL || args.modulePath == NULL || args.entryPointCount == 0 || args.ppEntryPointNames == NULL || args.pProgram == NULL) {
		return SLANG_E_INVALID_ARG;
	}

	for (uint32_t i = 0; i < args.entryPointCount; i++) {
		if (args.ppEntryPointNames[i] == NULL) {
			return SLANG_E_INVALID_ARG;
		}
	}

	VkToolkitSlangProgram program = {};

	SlangResult result = slang::createGlobalSession(program.globalSession.writeRef());
	if (SLANG_FAILED(result)) {
		return result;
	}

	result = program.globalSession->createSession(*args.pSessionDesc, program.session.writeRef());
	if (SLANG_FAILED(result)) {
		return result;
	}

	program.module = program.session->loadModule(args.modulePath, program.diagnostics.writeRef());
	vkToolkitLogSlangDiagnostics(program.diagnostics);
	if (program.module == NULL) {
		return SLANG_FAIL;
	}

	program.entryPoints.resize(args.entryPointCount);
	for (uint32_t i = 0; i < args.entryPointCount; i++) {
		result = program.module->findEntryPointByName(
			args.ppEntryPointNames[i],
			program.entryPoints[i].writeRef()
		);
		if (SLANG_FAILED(result)) {
			return result;
		}
	}

	std::vector<slang::IComponentType*> components(args.entryPointCount + 1);
	components[0] = program.module;
	for (uint32_t i = 0; i < args.entryPointCount; i++) {
		components[i + 1] = program.entryPoints[i];
	}

	Slang::ComPtr<slang::IComponentType> compositeProgram;
	result = program.session->createCompositeComponentType(
		components.data(),
		(SlangInt)components.size(),
		compositeProgram.writeRef(),
		program.diagnostics.writeRef()
	);
	vkToolkitLogSlangDiagnostics(program.diagnostics);
	if (SLANG_FAILED(result)) {
		return result;
	}

	result = compositeProgram->link(
		program.linkedProgram.writeRef(),
		program.diagnostics.writeRef()
	);
	vkToolkitLogSlangDiagnostics(program.diagnostics);
	if (SLANG_FAILED(result)) {
		return result;
	}

	program.pLayout = program.linkedProgram->getLayout(
		args.targetIndex,
		program.diagnostics.writeRef()
	);
	vkToolkitLogSlangDiagnostics(program.diagnostics);
	if (program.pLayout == NULL) {
		return SLANG_FAIL;
	}

	program.entryPointCodes.resize(args.entryPointCount);
	for (uint32_t i = 0; i < args.entryPointCount; i++) {
		result = program.linkedProgram->getEntryPointCode(
			(SlangInt)i,
			args.targetIndex,
			program.entryPointCodes[i].writeRef(),
			program.diagnostics.writeRef()
		);
		vkToolkitLogSlangDiagnostics(program.diagnostics);
		if (SLANG_FAILED(result)) {
			return result;
		}
	}

	*args.pProgram = std::move(program);
	return SLANG_OK;
}

#endif//__cplusplus

#endif//COMMON_H
