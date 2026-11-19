#include "compute.h"



int cleanupApplication(ApplicationResources resources, int exitCode) {
	Driver* pDriver = resources.pDriver;
	Commands* pCommands = resources.pCommands;

	if (pDriver->device != VK_NULL_HANDLE) {
		vkDeviceWaitIdle(pDriver->device);

		destroyDescriptors(pDriver->device, resources.pDescriptors);
		destroyPipeline(pDriver->device, resources.pPipeline);
		destroyMemory(pDriver->device, resources.pMemory);

		vkDestroyFence(pDriver->device, pCommands->fence, NULL);
		vkDestroyCommandPool(pDriver->device, pCommands->commandPool, NULL);
		vkDestroyDevice(pDriver->device, NULL);
	}

	if (pDriver->instance != VK_NULL_HANDLE) {
		vkDestroyInstance(pDriver->instance, NULL);
	}

	return exitCode;
}
