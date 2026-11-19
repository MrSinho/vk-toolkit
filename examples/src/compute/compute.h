#ifndef VK_TOOLKIT_COMPUTE_H
#define VK_TOOLKIT_COMPUTE_H

#include <stdint.h>
#include <vector>

#include <vk-bridge/vk-bridge.h>

#define VK_TOOLKIT_APP_NAME "vk-toolkit-compute"
#include "../common/common.h"

#define COMPUTE_VALUE_COUNT 64
#define COMPUTE_WORKGROUP_SIZE 64
#define COMPUTE_WORKGROUP_COUNT ((COMPUTE_VALUE_COUNT + COMPUTE_WORKGROUP_SIZE - 1) / COMPUTE_WORKGROUP_SIZE)
#define COMPUTE_DEVICE_BUFFER_COUNT 2
#define COMPUTE_ARRAY_COUNT(array) ((uint32_t)(sizeof(array) / sizeof((array)[0])))

typedef enum DeviceBufferIndex {
	COMPUTE_DEVICE_BUFFER_VALUES,
	COMPUTE_DEVICE_BUFFER_FACTOR
} DeviceBufferIndex;

typedef struct Driver {
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties physicalDeviceProperties;
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
	VkDevice device;
} Driver;

typedef struct Commands {
	uint32_t queueFamilyIndex;
	VkQueue queue;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
	VkFence fence;
} Commands;

typedef struct StagingLayout {
	VkDeviceSize valuesOffset;
	VkDeviceSize factorOffset;
	VkDeviceSize size;
} StagingLayout;

typedef struct Barriers {
	VkBufferMemoryBarrier2 transferToComputeMemoryBarriers[COMPUTE_DEVICE_BUFFER_COUNT];
	VkBufferMemoryBarrier2 computeToTransferMemoryBarrier;
	VkBufferMemoryBarrier2 transferToHostMemoryBarrier;
	VkDependencyInfo transferToComputeDependencyInfo;
	VkDependencyInfo computeToTransferDependencyInfo;
	VkDependencyInfo transferToHostDependencyInfo;
} Barriers;

typedef struct Memory {
	StagingLayout stagingLayout;
	VkShellBufferData stagingBuffer;
	VkShellBufferMemoryData stagingMemory;
	uint8_t* pMappedStaging;
	VkShellBufferData deviceBuffers[COMPUTE_DEVICE_BUFFER_COUNT];
	VkShellBufferMemoryData deviceMemory;
	VkBufferCopy stagingToDeviceLocalCopies[COMPUTE_DEVICE_BUFFER_COUNT];
	VkBufferCopy deviceLocalToStagingCopy;
	Barriers barriers;
} Memory;

typedef struct CreateMemoryArgs {
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	const float* pValues;
	uint32_t valueCount;
	float factor;
	Memory* pMemory;
} CreateMemoryArgs;

VkResult createMemory(CreateMemoryArgs args);
void copyBuffers(VkCommandBuffer commandBuffer, const Memory* pMemory);
void applyBarriers(VkCommandBuffer commandBuffer, const Memory* pMemory);
void destroyMemory(VkDevice device, Memory* pMemory);

typedef struct Pipeline {
	VkBridgeShaderStageReflection shaderReflection;
	VkBridgePipelineReflection pipelineReflection;
	std::vector<VkDescriptorSetLayout> setLayouts;
	VkPipelineLayout layout;
	VkPipeline pipeline;
} Pipeline;

uint8_t createPipeline(VkDevice device, Pipeline* pPipeline);
void destroyPipeline(VkDevice device, Pipeline* pPipeline);

typedef struct Descriptors {
	VkDescriptorPool pool;
	std::vector<VkDescriptorSet> sets;
	VkBridgeDescriptorSetLayoutBinding valuesBinding;
	VkBridgeDescriptorSetLayoutBinding factorBinding;
} Descriptors;

typedef struct CreateDescriptorsArgs {
	VkDevice device;
	const Memory* pMemory;
	const Pipeline* pPipeline;
	Descriptors* pDescriptors;
} CreateDescriptorsArgs;

uint8_t createDescriptors(CreateDescriptorsArgs args);
void destroyDescriptors(VkDevice device, Descriptors* pDescriptors);

typedef struct ApplicationResources {
	Driver* pDriver;
	Commands* pCommands;
	Memory* pMemory;
	Pipeline* pPipeline;
	Descriptors* pDescriptors;
} ApplicationResources;

uint8_t initializeVulkan(Driver* pDriver, Commands* pCommands);
int cleanupApplication(ApplicationResources resources, int exitCode);

#endif//VK_TOOLKIT_COMPUTE_H
