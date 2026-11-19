#include "compute.h"

#include "../common/bridge.h"
#include "../common/pipeline.h"
#include "../common/shader.h"

#include <filesystem>
#include <string>
#include <utility>

typedef struct PipelineCreationResources {
	VkDevice device;
	SpvReflectShaderModule* pSpvModule;
	const uint8_t* pSpvModuleCreated;
	const VkShaderModule* pShaderModule;
	VkBridgeShaderStageReflection* pShaderReflection;
	VkBridgePipelineReflection* pPipelineReflection;
	std::vector<VkDescriptorSetLayout>* pSetLayouts;
	VkPipelineLayout* pPipelineLayout;
	const VkPipeline* pPipeline;
} PipelineCreationResources;

static uint8_t cleanupPipelineCreation(PipelineCreationResources resources) {
	vkDestroyPipeline(resources.device, *resources.pPipeline, NULL);
	vkToolkitDestroyPipelineLayout(
		resources.device,
		resources.pSetLayouts,
		resources.pPipelineLayout
	);

	vkBridgeDestroyPipelineReflection(resources.pPipelineReflection);
	vkBridgeDestroyShaderStageReflection(resources.pShaderReflection);
	vkDestroyShaderModule(resources.device, *resources.pShaderModule, NULL);

	if (*resources.pSpvModuleCreated) {
		spvReflectDestroyShaderModule(resources.pSpvModule);
	}

	return 0;
}

uint8_t createPipeline(VkDevice device, Pipeline* pPipeline) {
	if (device == VK_NULL_HANDLE || pPipeline == NULL) {
		return 0;
	}

	SpvReflectShaderModule spvModule = {};
	uint8_t spvModuleCreated = 0;
	VkShaderModule shaderModule = VK_NULL_HANDLE;
	VkBridgeShaderStageReflection shaderReflection = {};
	VkBridgePipelineReflection pipelineReflection = {};
	std::vector<VkDescriptorSetLayout> setLayouts;
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline pipeline = VK_NULL_HANDLE;
	PipelineCreationResources creationResources = {
		.device = device,
		.pSpvModule = &spvModule,
		.pSpvModuleCreated = &spvModuleCreated,
		.pShaderModule = &shaderModule,
		.pShaderReflection = &shaderReflection,
		.pPipelineReflection = &pipelineReflection,
		.pSetLayouts = &setLayouts,
		.pPipelineLayout = &pipelineLayout,
		.pPipeline = &pipeline
	};

	slang::TargetDesc targetDesc = {
		.format = SLANG_SPIRV
	};

	slang::SessionDesc sessionDesc = {
		.targets = &targetDesc,
		.targetCount = 1
	};

	std::string shaderPath = vkToolkitGetShaderPath(
		VK_TOOLKIT_INSTALL_DATADIR,
		"shaders/power.slang",
		std::filesystem::path(__FILE__).parent_path() / "../../share/shaders/power.slang"
	);

	const char* entryPointNames[1] = { "computeMain" };
	VkToolkitSlangProgram slangProgram = {};
	VK_TOOLKIT_SLANG_CHECK(
		vkToolkitCompileSlangProgram((VkToolkitCompileSlangProgramArgs) {
			.pSessionDesc = &sessionDesc,
			.modulePath = shaderPath.c_str(),
			.entryPointCount = COMPUTE_ARRAY_COUNT(entryPointNames),
			.ppEntryPointNames = entryPointNames,
			.targetIndex = 0,
			.pProgram = &slangProgram
		}),
		return cleanupPipelineCreation(creationResources)
	);

	Slang::ComPtr<slang::IBlob>& spirv = slangProgram.entryPointCodes[0];
	const size_t spirvSize = spirv->getBufferSize();
	const void* spirvBytes = spirv->getBufferPointer();

	VK_TOOLKIT_SPV_REFLECT_CHECK(
		spvReflectCreateShaderModule(spirvSize, spirvBytes, &spvModule),
		return cleanupPipelineCreation(creationResources)
	);

	spvModuleCreated = 1;

	VkShaderModuleCreateInfo shaderModuleCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.codeSize = spirvSize,
		.pCode = (const uint32_t*)spirvBytes
	};
	VK_TOOLKIT_VK_RESULT_ERROR(
		vkCreateShaderModule(device, &shaderModuleCreateInfo, NULL, &shaderModule),
		return cleanupPipelineCreation(creationResources)
	);

	VK_TOOLKIT_BRIDGE_CHECK(vkBridgeReflectShaderStage((VkBridgeReflectShaderStageArgs) {
		.shaderStage = VK_SHADER_STAGE_COMPUTE_BIT,
		.pSpvModule = &spvModule,
		.shaderInputsExcludeDecorationFlags = 0,
		.shaderInputAdditionalInfoCount = 0,
		.pShaderInputAdditionalInfos = NULL,
		.specializationConstantSourceCount = 0,
		.pSpecializationConstantSources = NULL,
		.pSpecializationData = NULL,
		.pReflection = &shaderReflection
	}), return cleanupPipelineCreation(creationResources));

	const VkBridgeShaderStageReflection* reflections[1] = { &shaderReflection };
	VK_TOOLKIT_BRIDGE_CHECK(vkBridgeMergeShaderStageReflections((VkBridgeMergeShaderStageReflectionsArgs) {
		.shaderStageCount = COMPUTE_ARRAY_COUNT(reflections),
		.ppShaderStageReflections = reflections,
		.pPipelineReflection = &pipelineReflection
	}), return cleanupPipelineCreation(creationResources));

	if (!vkToolkitCreatePipelineLayout((VkToolkitCreatePipelineLayoutArgs) {
		.device = device,
		.pReflection = &pipelineReflection,
		.pSetLayouts = &setLayouts,
		.pPipelineLayout = &pipelineLayout
	})) {
		return cleanupPipelineCreation(creationResources);
	}

	VkPipelineShaderStageCreateInfo shaderStage = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.stage = VK_SHADER_STAGE_COMPUTE_BIT,
		.module = shaderModule,
		.pName = spvModule.entry_point_name,
		.pSpecializationInfo = NULL
	};

	VkComputePipelineCreateInfo pipelineCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.stage = shaderStage,
		.layout = pipelineLayout,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};
	VK_TOOLKIT_VK_RESULT_ERROR(
		vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, NULL, &pipeline),
		return cleanupPipelineCreation(creationResources)
	);

	vkDestroyShaderModule(device, shaderModule, NULL);
	spvReflectDestroyShaderModule(&spvModule);
	shaderModule = VK_NULL_HANDLE;
	spvModuleCreated = 0;

	pPipeline->shaderReflection = shaderReflection;
	pPipeline->pipelineReflection = pipelineReflection;
	pPipeline->setLayouts = std::move(setLayouts);
	pPipeline->layout = pipelineLayout;
	pPipeline->pipeline = pipeline;
	return 1;

}

void destroyPipeline(VkDevice device, Pipeline* pPipeline) {
	if (device == VK_NULL_HANDLE || pPipeline == NULL) {
		return;
	}

	vkDestroyPipeline(device, pPipeline->pipeline, NULL);
	vkToolkitDestroyPipelineLayout(device, &pPipeline->setLayouts, &pPipeline->layout);

	vkBridgeDestroyPipelineReflection(&pPipeline->pipelineReflection);
	vkBridgeDestroyShaderStageReflection(&pPipeline->shaderReflection);

	*pPipeline = Pipeline {};
}
