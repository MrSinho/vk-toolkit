#ifndef VK_SHELL_H
#define VK_SHELL_H



#ifdef __cplusplus
extern "C" {
#endif//__cplusplus



#include <vulkan/vulkan.h>
#include <stdint.h>
#include <stdlib.h>



typedef enum VkShellResult {
	VK_SHELL_SUCCESS = 0,
	VK_SHELL_ERROR_INVALID_ARGUMENT = -1,
	VK_SHELL_ERROR_OUT_OF_HOST_MEMORY = -2,
	VK_SHELL_ERROR_NOT_FOUND = -3,
	VK_SHELL_ERROR_UNSUPPORTED = -4,
	VK_SHELL_ERROR_BUFFER_TOO_SMALL = -5,
	VK_SHELL_ERROR_LIMIT_EXCEEDED = -6,
	VK_SHELL_ERROR_VULKAN = -7
} VkShellResult;

extern const char* vkShellTranslateResult(VkShellResult result);

#define VK_SHELL_CHECK(functionCall, failureExpression)\
	do {\
		const VkShellResult vkShellCheckedResult = (functionCall);\
		if (vkShellCheckedResult != VK_SHELL_SUCCESS) {\
			failureExpression;\
		}\
	} while (0)

extern const char* vkShellTranslateVkResult(
	VkResult vkResult
);

#define VK_SHELL_MAX_STACK_VALIDATION_LAYER_COUNT 32

#define VK_SHELL_MAX_STACK_QUEUE_FAMILY_COUNT          32
#define VK_SHELL_MAX_STACK_PHYSICAL_DEVICE_COUNT       8
#define VK_SHELL_MAX_STACK_QUEUE_COUNT                 64
#define VK_SHELL_MAX_STACK_DEVICE_SURFACE_FORMAT_COUNT 256
#define VK_SHELL_MAX_STACK_SURFACE_PRESENT_MODE_COUNT  16

extern VkShellResult vkShellFindValidationLayer(
	const char* validationLayerName,
	uint8_t* pFound
);

typedef struct VkShellCreateInstanceArgs {
	const char*    applicationName;
	const char*    engineName;
	const uint8_t  enableValidationLayers;
	const uint32_t extensionCount;
	const char**   ppExtensionNames;
	uint32_t       apiVersion;
	VkInstance*    pInstance;
} VkShellCreateInstanceArgs;

extern VkResult vkShellCreateInstance(VkShellCreateInstanceArgs args);

typedef struct VkShellGetPhysicalDeviceQueueFamiliesArgs {
	VkPhysicalDevice         physicalDevice;
	VkSurfaceKHR             surface;
	uint32_t*                pQueueFamilyCount;
	uint32_t*                pGraphicsQueueFamilyCount;
	uint32_t*                pSurfaceQueueFamilyCount;
	uint32_t*                pComputeQueueFamilyCount;
	uint32_t*                pTransferQueueFamilyCount;
	uint32_t*                pGraphicsQueueFamilyIndices;
	uint32_t*                pSurfaceQueueFamilyIndices;
	uint32_t*                pComputeQueueFamilyIndices;
	uint32_t*                pTransferQueueFamilyIndices;
	VkQueueFamilyProperties* pQueueFamiliesProperties;
} VkShellGetPhysicalDeviceQueueFamiliesArgs;

extern VkShellResult vkShellGetPhysicalDeviceQueueFamilies(VkShellGetPhysicalDeviceQueueFamiliesArgs args);

typedef struct VkShellSelectPhysicalDeviceArgs {
	VkInstance                        instance;
	VkSurfaceKHR                      surface;
	VkQueueFlags                      queuesRequirements;
	uint8_t                           dynamicRendering;
	uint8_t						      synchronization2;
	VkPhysicalDevice*                 pPhysicalDevice;
	VkPhysicalDeviceProperties*       pPhysicalDeviceProperties;
	VkPhysicalDeviceFeatures2*        pPhysicalDeviceFeatures2;
	VkPhysicalDeviceMemoryProperties* pPhysicalDeviceMemoryProperties;
} VkShellSelectPhysicalDeviceArgs;

extern VkShellResult vkShellSelectPhysicalDevice(VkShellSelectPhysicalDeviceArgs args);

extern VkResult vkShellGetPhysicalDeviceSurfaceCapabilities(
	VkPhysicalDevice          physicalDevice,
	VkSurfaceKHR              surface,
	VkSurfaceCapabilitiesKHR* pSurfaceCapabilities,
	uint32_t                  clampCurrentExtentWidthValue,
	uint32_t                  clampCurrentExtentHeightValue
);


typedef struct VkShellSetLogicalDeviceArgs {
	VkPhysicalDevice         physicalDevice;
	VkDevice*                pDevice;
	uint32_t                 extensionCount;
	const char* const*       ppExtensionNames;
	uint32_t                 deviceQueueCount;
	const VkDeviceQueueCreateInfo* pDeviceQueueInfos;
} VkShellSetLogicalDeviceArgs;

extern VkResult vkShellSetLogicalDevice(VkShellSetLogicalDeviceArgs args);


extern VkShellResult vkShellCheckSupportedDeviceColorFormat(
	VkPhysicalDevice physicalDevice,
	VkFormat         format,
	uint8_t*         pColorAttachmentSupported
);

#define VK_SHELL_MAX_STACK_DEVICE_COLOR_FORMATS_QUERIES 44
#define VK_SHELL_FORMAT_UINT      0
#define VK_SHELL_FORMAT_SINT      1
#define VK_SHELL_FORMAT_SFLOAT    2
#define VK_SHELL_FORMAT_UNDEFINED 3

typedef enum VkShellImageChannelTypeFlags {
	VK_SHELL_IMAGE_CHANNEL_TYPE_UNORM     = 1 << 0,
	VK_SHELL_IMAGE_CHANNEL_TYPE_SNORM     = 1 << 1,
	VK_SHELL_IMAGE_CHANNEL_TYPE_UINT      = 1 << 2,
	VK_SHELL_IMAGE_CHANNEL_TYPE_SINT      = 1 << 3,
	VK_SHELL_IMAGE_CHANNEL_TYPE_SFLOAT    = 1 << 4,
	VK_SHELL_IMAGE_CHANNEL_TYPE_UNDEFINED = 0,
} VkShellImageChannelTypeFlags;

extern VkShellResult vkShellFindSupportedDeviceColorFormats(
	VkPhysicalDevice             physicalDevice,
	uint32_t                     minChannelCount,
	uint32_t                     maxChannelCount,
	uint32_t                     minChannelSize,
	uint32_t                     maxChannelSize,
	VkShellImageChannelTypeFlags channelTypes,
	uint32_t*                    pSupportedFormatCount,
	VkFormat*                    pSupportedFormats,
	uint32_t*                    pChannelsCount,
	uint32_t*                    pSingleChannelsSizes,
	uint32_t*                    pChannelsTypes
);

typedef struct VkShellCreateSwapchainArgs {
	VkDevice                 device;
	VkPhysicalDevice         physicalDevice;
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	VkSurfaceKHR             surface;
	VkFormat                 imageFormat;
	VkFormat*                pImageFormat;
	uint32_t                 swapchainImageCount;
	VkSharingMode            imageSharingMode;
	uint32_t                 queueFamilyIndexCount;
	const uint32_t*          pQueueFamilyIndices;
	uint8_t                  vsync;
	uint32_t*                pSwapchainImageCount;
	VkSwapchainKHR           oldSwapchain;
	VkSwapchainKHR*          pSwapchain;
} VkShellCreateSwapchainArgs;

extern VkResult vkShellCreateSwapchain(VkShellCreateSwapchainArgs args);

extern VkShellResult vkShellCombineMaxSamples(
	VkPhysicalDeviceProperties physicalDeviceProperties,
	uint32_t                   sampleCount,
	uint8_t                    combineColorSample,
	uint8_t                    combineDepthSample,
	uint32_t*                  pSampleCount
);

extern VkResult vkShellDestroyFramebuffers(
	VkDevice       device,
	uint32_t       framebufferCount,
	VkFramebuffer* pFramebuffers
);

extern VkResult vkShellQueueSubmit(
	uint32_t             cmdBufferCount,
	VkCommandBuffer*     pCmdBuffers,
	VkQueue              queue,
	VkFence              fence,
	uint32_t             semaphoresToWaitForCount,
	VkSemaphore*         pSemaphoresToWaitFor,
	VkPipelineStageFlags waitStage,
	uint32_t             signalSemaphoreCount,
	VkSemaphore*         pSignalSemaphores
);

extern VkResult vkShellWaitForQueue(
	VkQueue queue
);

extern VkResult vkShellCreateFences(
	VkDevice device,
	uint32_t fenceCount,
	uint8_t  signaled,
	VkFence* pFences
);

extern VkResult vkShellCreateSemaphores(
	VkDevice     device,
	uint32_t     semaphoreCount,
	VkSemaphore* pSemaphores
);

#define VK_SHELL_VEC1_SIGNED_FLOAT		VK_FORMAT_R32_SFLOAT
#define VK_SHELL_VEC2_SIGNED_FLOAT		VK_FORMAT_R32G32_SFLOAT
#define VK_SHELL_VEC3_SIGNED_FLOAT		VK_FORMAT_R32G32B32_SFLOAT
#define VK_SHELL_VEC4_SIGNED_FLOAT		VK_FORMAT_R32G32B32A32_SFLOAT

#define VK_SHELL_VEC1_SIGNED_DOUBLE		VK_FORMAT_R64_SFLOAT
#define VK_SHELL_VEC2_SIGNED_DOUBLE		VK_FORMAT_R64G64_SFLOAT
#define VK_SHELL_VEC3_SIGNED_DOUBLE		VK_FORMAT_R64G64B64_SFLOAT
#define VK_SHELL_VEC4_SIGNED_DOUBLE		VK_FORMAT_R64G64B64A64_SFLOAT

#define VK_SHELL_VEC1_SIGNED_INT		VK_FORMAT_R32_SINT
#define VK_SHELL_VEC2_SIGNED_INT		VK_FORMAT_R32G32_SINT
#define VK_SHELL_VEC3_SIGNED_INT		VK_FORMAT_R32G32B32_SINT
#define VK_SHELL_VEC4_SIGNED_INT		VK_FORMAT_R32G32B32A32_SINT

#define VK_SHELL_VEC1_UNSIGNED_INT		VK_FORMAT_R32_UINT
#define VK_SHELL_VEC2_UNSIGNED_INT		VK_FORMAT_R32G32_UINT
#define VK_SHELL_VEC3_UNSIGNED_INT		VK_FORMAT_R32G32B32_UINT
#define VK_SHELL_VEC4_UNSIGNED_INT		VK_FORMAT_R32G32B32A32_UINT

#define VK_SHELL_VEC1_UNSIGNED_LONG		VK_FORMAT_R64_UINT
#define VK_SHELL_VEC2_UNSIGNED_LONG		VK_FORMAT_R64G64_UINT
#define VK_SHELL_VEC3_UNSIGNED_LONG		VK_FORMAT_R64G64B64_UINT
#define VK_SHELL_VEC4_UNSIGNED_LONG		VK_FORMAT_R64G64B64A64_UINT

static VkDeviceSize vkShellGetNextBlockOffset(VkDeviceSize previousBlockSize, VkDeviceSize nextBlockAlignment) {
	return (previousBlockSize + nextBlockAlignment - 1) / nextBlockAlignment * nextBlockAlignment;
}

static VkDeviceSize vkShellGetPadding(VkDeviceSize previousBlockSize, VkDeviceSize nextBlockAlignment) {
	return vkShellGetNextBlockOffset(previousBlockSize, nextBlockAlignment) - previousBlockSize;
}

typedef struct VkShellBufferData {
	VkBuffer buffer;
	VkDeviceSize size;
	VkMemoryRequirements memoryRequirements;
	VkDeviceSize memoryOffset;
} VkShellBufferData;

typedef struct VkShellBufferMemoryData {
	VkDeviceMemory memory;
	VkDeviceSize allocationSize;
	uint32_t memoryTypeBits;
	uint32_t memoryTypeIndex;
	void* pMapped;
	uint32_t bufferCount;
	VkShellBufferData* pBuffers;
} VkShellBufferMemoryData;

typedef struct VkShellGetBufferMemoryLayoutArgs {
	uint32_t bufferCount;
	VkShellBufferData* pBuffers;
	VkShellBufferMemoryData* pMemory;
} VkShellGetBufferMemoryLayoutArgs;

extern VkShellResult vkShellGetBufferMemoryLayout(
	VkShellGetBufferMemoryLayoutArgs args
);

extern VkShellResult vkShellGetMemoryTypeIndex(
	VkDevice              device,
	VkPhysicalDevice      physicalDevice,
	uint32_t              memoryTypeBits,
	VkMemoryPropertyFlags propertyFlags,
	uint32_t*             pMemoryTypeIndex
);

extern VkShellResult vkShellGetMemoryBudgetProperties(
	VkPhysicalDevice                           physicalDevice,
	VkPhysicalDeviceMemoryBudgetPropertiesEXT* pMemoryBudgetProperties
);

static VkBool32 vkShellSampleShadingEnable(float minSampleShadingSize) {
	return minSampleShadingSize == 0.0f ? VK_FALSE : VK_TRUE;
}



#ifdef __cplusplus
}
#endif//__cplusplus

#endif//VK_SHELL_H
