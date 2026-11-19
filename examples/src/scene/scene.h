#ifndef VK_TOOLKIT_SCENE_H
#define VK_TOOLKIT_SCENE_H

#include <stdint.h>
#include <vector>

#include <vk-bridge/vk-bridge.h>

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

#define VK_TOOLKIT_APP_NAME "vk-toolkit-scene"
#include "../common/common.h"
#include "../common/image.h"

#define QUAD_VERTEX_COUNT     4
#define TRIANGLE_VERTEX_COUNT 3
#define QUAD_INDEX_COUNT      6
#define SCENE_MAX_FRAMES_IN_FLIGHT 2
#define SCENE_MAX_RENDERING_RESOURCE_COUNT (SCENE_MAX_FRAMES_IN_FLIGHT * MAX_STACK_SWAPCHAIN_IMAGE_COUNT)
#define ARRAY_COUNT(array) ((uint32_t)(sizeof(array) / sizeof((array)[0])))

typedef struct Vertex {
	float position[3];
} Vertex;

extern Vertex quad[QUAD_VERTEX_COUNT];
extern Vertex triangle[TRIANGLE_VERTEX_COUNT];
extern float models[48];
extern uint32_t indices[QUAD_INDEX_COUNT];
extern float light[8];
extern float projectionView[32];

typedef enum DeviceBufferIndex {
	SCENE_DEVICE_BUFFER_VERTEX,
	SCENE_DEVICE_BUFFER_INSTANCE,
	SCENE_DEVICE_BUFFER_INDEX,
	SCENE_DEVICE_BUFFER_UNIFORM,
	SCENE_DEVICE_BUFFER_COUNT
} DeviceBufferIndex;

typedef struct StagingLayout {
	VkDeviceSize quadVerticesOffset;
	VkDeviceSize triangleVerticesOffset;
	VkDeviceSize instanceModelsOffset;
	VkDeviceSize quadIndicesOffset;
	VkDeviceSize lightOffset;
	VkDeviceSize size;
} StagingLayout;

typedef struct Barriers {
	VkBufferMemoryBarrier2 transferToVertexMemoryBarriers[2];
	VkBufferMemoryBarrier2 transferToIndexMemoryBarrier;
	VkBufferMemoryBarrier2 transferToUniformMemoryBarrier;
	VkDependencyInfo transferToVertexDependencyInfo;
	VkDependencyInfo transferToIndexDependencyInfo;
	VkDependencyInfo transferToUniformDependencyInfo;
} Barriers;

typedef struct Memory {
	StagingLayout stagingLayout;
	VkShellBufferData stagingBuffer;
	VkShellBufferMemoryData stagingMemory;
	uint8_t* pMappedStaging;
	VkShellBufferData deviceLocalBuffers[SCENE_DEVICE_BUFFER_COUNT];
	VkShellBufferMemoryData deviceLocalMemory;
	VkBufferCopy bufferCopies[SCENE_DEVICE_BUFFER_COUNT];
	VkBufferCopy triangleBufferCopies[SCENE_MAX_FRAMES_IN_FLIGHT];
	uint32_t frameCount;
	Barriers barriers;
} Memory;

typedef struct CreateMemoryArgs {
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	uint32_t frameCount;
	Memory* pMemory;
} CreateMemoryArgs;

uint8_t createMemory(CreateMemoryArgs args);
void destroyMemory(VkDevice device, Memory* pMemory);
void copyBuffers(VkCommandBuffer commandBuffer, const Memory* pMemory);
void applyBarriers(VkCommandBuffer commandBuffer, const Memory* pMemory);

typedef struct RenderingMemory {
	VkToolkitImageData colorImages[SCENE_MAX_FRAMES_IN_FLIGHT];
	VkToolkitImageData depthImages[SCENE_MAX_FRAMES_IN_FLIGHT];
	VkDeviceMemory memory;
	uint32_t frameCount;
} RenderingMemory;

typedef struct RenderingBarriers {
	VkImageMemoryBarrier2 msaaToColorAttachmentMemoryBarrier;
	VkImageMemoryBarrier2 swapchainToColorAttachmentMemoryBarrier;
	VkImageMemoryBarrier2 depthToDepthAttachmentMemoryBarrier;
	VkDependencyInfo msaaToColorAttachmentDependencyInfo;
	VkDependencyInfo swapchainToColorAttachmentDependencyInfo;
	VkDependencyInfo depthToDepthAttachmentDependencyInfo;
} RenderingBarriers;

typedef struct RenderingData {
	VkRenderingAttachmentInfo colorAttachmentInfo;
	VkRenderingAttachmentInfo depthAttachmentInfo;
	VkRenderingInfo renderingInfo;
	RenderingBarriers barriers;
} RenderingData;

typedef struct RenderingResources {
	RenderingMemory memory;
	RenderingData data[SCENE_MAX_RENDERING_RESOURCE_COUNT];
} RenderingResources;

typedef struct CreateRenderingResourcesArgs {
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkExtent2D extent;
	VkFormat colorFormat;
	VkFormat depthFormat;
	VkSampleCountFlagBits sampleCount;
	uint32_t frameCount;
	uint32_t swapchainImageCount;
	uint32_t swapchainImageCapacity;
	const VkImage* pSwapchainImages;
	const VkImageView* pSwapchainImageViews;
	RenderingResources* pResources;
} CreateRenderingResourcesArgs;

uint8_t createRenderingResources(CreateRenderingResourcesArgs args);
void destroyRenderingResources(VkDevice device, RenderingResources* pResources);

typedef struct Pipeline {
	VkBridgeShaderStageReflection vertexReflection;
	VkBridgeShaderStageReflection fragmentReflection;
	VkBridgePipelineReflection pipelineReflection;
	std::vector<VkDescriptorSetLayout> setLayouts;
	VkPipelineLayout layout;
	VkPipeline pipeline;
} Pipeline;

typedef struct CreatePipelineArgs {
	VkDevice device;
	VkSampleCountFlagBits sampleCount;
	VkFormat colorFormat;
	VkFormat depthFormat;
	Pipeline* pPipeline;
} CreatePipelineArgs;

uint8_t createPipeline(CreatePipelineArgs args);
void destroyPipeline(VkDevice device, Pipeline* pPipeline);

typedef struct Descriptors {
	VkDescriptorPool pool;
	std::vector<VkDescriptorSet> sets;
	VkBridgeDescriptorSetLayoutBinding lightBinding;
	uint32_t frameSetStride;
} Descriptors;

typedef struct CreateDescriptorsArgs {
	VkDevice device;
	uint32_t frameCount;
	VkBuffer lightBuffer;
	VkDeviceSize lightBufferSize;
	const Pipeline* pPipeline;
	Descriptors* pDescriptors;
} CreateDescriptorsArgs;

uint8_t createDescriptors(CreateDescriptorsArgs args);
void destroyDescriptors(VkDevice device, Descriptors* pDescriptors);

VkResult recreateSwapchain(
	int width,
	int height,
	VkSampleCountFlagBits sampleCount,
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
	Memory* pMemory,
	RenderingResources* pRenderingResources,
	Pipeline* pPipeline,
	Descriptors* pDescriptors,
	int exitCode
);

#endif//VK_TOOLKIT_SCENE_H
