#include "clear-color.h"

int cleanup(
	GLFWwindow* window,
	Driver* pDriver,
	Queues* pQueues,
	Commands* pCommands,
	SurfaceData* pSurfaceData,
	SwapchainData* pSwapchainData,
	int exitCode
) {
	if (pDriver->device != VK_NULL_HANDLE) {
		vkDeviceWaitIdle(pDriver->device);

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroyFence(pDriver->device, pCommands->graphicsCmdFences[i], NULL);
			vkDestroySemaphore(pDriver->device, pQueues->imageAcquiredSemaphores[i], NULL);
		}

		for (uint32_t i = 0; i < MAX_STACK_SWAPCHAIN_IMAGE_COUNT; i++) {
			vkDestroySemaphore(pDriver->device, pQueues->graphicsQueueFinishedSemaphores[i], NULL);
			vkDestroyImageView(pDriver->device, pSwapchainData->swapchainImageViews[i], NULL);
		}

		vkDestroyCommandPool(pDriver->device, pCommands->graphicsCmdPool, NULL);
		vkDestroySwapchainKHR(pDriver->device, pSwapchainData->swapchain, NULL);
		vkDestroyDevice(pDriver->device, NULL);
	}

	if (pDriver->instance != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(pDriver->instance, pSurfaceData->surface, NULL);
		vkDestroyInstance(pDriver->instance, NULL);
	}

	if (window != NULL) {
		glfwDestroyWindow(window);
	}

	glfwTerminate();

	return exitCode;
}
