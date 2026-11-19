#ifndef VK_TOOLKIT_CLEAR_COLOR_H
#define VK_TOOLKIT_CLEAR_COLOR_H

#include <vk-shell/vk-shell.h>

#ifndef GRAPHICS_EXAMPLE
	#define GRAPHICS_EXAMPLE
#endif//GRAPHICS_EXAMPLE

#define SWAPCHAIN_IMAGE_COUNT 1
#define MAX_STACK_SWAPCHAIN_IMAGE_COUNT 16
#define MAX_FRAMES_IN_FLIGHT 2

typedef struct Driver {
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties physicalDeviceProperties;
	VkDevice device;
} Driver;

typedef struct Queues {
	uint32_t graphicsQueueFamilyIndex;
	uint32_t presentQueueFamilyIndex;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSemaphore imageAcquiredSemaphores[MAX_FRAMES_IN_FLIGHT];
	VkSemaphore graphicsQueueFinishedSemaphores[MAX_STACK_SWAPCHAIN_IMAGE_COUNT];
} Queues;

#define VK_TOOLKIT_APP_NAME "vk-toolkit-clear-color"
#include "../common/common.h"

typedef struct RenderingResources {
	VkRenderingAttachmentInfo colorAttachmentInfo;
	VkRenderingInfo renderingInfo;
	VkImageMemoryBarrier2 toColorAttachmentMemoryBarrier;
	VkDependencyInfo toColorAttachmentDependencyInfo;
} RenderingResources;

uint8_t createRenderingResources(
	VkExtent2D extent,
	uint32_t swapchainImageCount,
	uint32_t resourceCapacity,
	const VkImage* pSwapchainImages,
	const VkImageView* pSwapchainImageViews,
	RenderingResources* pResources
);

VkResult recreateSwapchain(
	int width,
	int height,
	uint8_t vsync,
	Driver* pDriver,
	Queues* pQueues,
	SurfaceData* pSurfaceData,
	SwapchainData* pSwapchainData,
	FrameData* pFrameData,
	RenderingResources* pRenderingResources
);

int cleanup(
	GLFWwindow* window,
	Driver* pDriver,
	Queues* pQueues,
	Commands* pCommands,
	SurfaceData* pSurfaceData,
	SwapchainData* pSwapchainData,
	int exitCode
);

#endif//VK_TOOLKIT_CLEAR_COLOR_H
