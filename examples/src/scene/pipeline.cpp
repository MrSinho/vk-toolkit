#include "scene.h"

#include "../common/bridge.h"
#include "../common/pipeline.h"
#include "../common/shader.h"

#include <filesystem>
#include <string>
#include <utility>

typedef struct PipelineCreationResources {
	VkDevice device;
	SpvReflectShaderModule* pVertexSpvModule;
	SpvReflectShaderModule* pFragmentSpvModule;
	const uint8_t* pVertexSpvModuleCreated;
	const uint8_t* pFragmentSpvModuleCreated;
	const VkShaderModule* pVertexVkModule;
	const VkShaderModule* pFragmentVkModule;
	VkBridgeShaderStageReflection* pVertexReflection;
	VkBridgeShaderStageReflection* pFragmentReflection;
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
	vkBridgeDestroyShaderStageReflection(resources.pFragmentReflection);
	vkBridgeDestroyShaderStageReflection(resources.pVertexReflection);
	vkDestroyShaderModule(resources.device, *resources.pFragmentVkModule, NULL);
	vkDestroyShaderModule(resources.device, *resources.pVertexVkModule, NULL);

	if (*resources.pFragmentSpvModuleCreated) {
		spvReflectDestroyShaderModule(resources.pFragmentSpvModule);
	}
	if (*resources.pVertexSpvModuleCreated) {
		spvReflectDestroyShaderModule(resources.pVertexSpvModule);
	}

	return 0;
}

uint8_t createPipeline(CreatePipelineArgs args) {
	if (args.device == VK_NULL_HANDLE || args.colorFormat == VK_FORMAT_UNDEFINED ||
		args.depthFormat == VK_FORMAT_UNDEFINED || args.sampleCount == 0 || args.pPipeline == NULL) {
		return 0;
	}

	SpvReflectShaderModule vertexSpvModule = {};
	SpvReflectShaderModule fragmentSpvModule = {};

	uint8_t vertexSpvModuleCreated = 0;
	uint8_t fragmentSpvModuleCreated = 0;

	VkShaderModule vertexVkModule = VK_NULL_HANDLE;
	VkShaderModule fragmentVkModule = VK_NULL_HANDLE;

	VkBridgeShaderStageReflection vertexReflection = {};
	VkBridgeShaderStageReflection fragmentReflection = {};
	VkBridgePipelineReflection pipelineReflection = {};

	std::vector<VkDescriptorSetLayout> setLayouts;
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

	VkPipeline graphicsPipeline = VK_NULL_HANDLE;
	PipelineCreationResources creationResources = {
		.device = args.device,
		.pVertexSpvModule = &vertexSpvModule,
		.pFragmentSpvModule = &fragmentSpvModule,
		.pVertexSpvModuleCreated = &vertexSpvModuleCreated,
		.pFragmentSpvModuleCreated = &fragmentSpvModuleCreated,
		.pVertexVkModule = &vertexVkModule,
		.pFragmentVkModule = &fragmentVkModule,
		.pVertexReflection = &vertexReflection,
		.pFragmentReflection = &fragmentReflection,
		.pPipelineReflection = &pipelineReflection,
		.pSetLayouts = &setLayouts,
		.pPipelineLayout = &pipelineLayout,
		.pPipeline = &graphicsPipeline
	};


	char logMessage[1024] = { 0 };
	slang::PreprocessorMacroDesc preprocessorMacros[2] = {
		(slang::PreprocessorMacroDesc) {
			.name  = "BIAS_VALUE",
			.value = "1138"
		},

		(slang::PreprocessorMacroDesc) {
			.name  = "OTHER_MACRO",
			.value = "float"
		}
	};

	slang::TargetDesc targetDesc = {
		.format = SLANG_SPIRV
	};

	slang::SessionDesc sessionDesc = {
		.targets = &targetDesc,
		.targetCount = 1,

		.preprocessorMacros = preprocessorMacros,
		.preprocessorMacroCount = 2,
	};

	if (VK_TOOLKIT_INSTALL_DATADIR[0] == '\0') {
		VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_STATUS, "VK_TOOLKIT_INSTALL_DATADIR is empty, using the source shader path");
	}

	std::string shaderPath = vkToolkitGetShaderPath(
		VK_TOOLKIT_INSTALL_DATADIR,
		"shaders/merged.slang",
		std::filesystem::path(__FILE__).parent_path() / "../../share/shaders/merged.slang"
	);

	snprintf(logMessage, sizeof(logMessage), "Shader path: %s", shaderPath.c_str());
	VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_DEBUG, logMessage);

	const char* entryPointNames[2] = {
		"vertexMain",
		"fragmentMain"
	};

	VkToolkitSlangProgram slangProgram = {};
	VK_TOOLKIT_SLANG_CHECK(
		vkToolkitCompileSlangProgram((VkToolkitCompileSlangProgramArgs) {
			.pSessionDesc = &sessionDesc,
			.modulePath = shaderPath.c_str(),
			.entryPointCount = ARRAY_COUNT(entryPointNames),
			.ppEntryPointNames = entryPointNames,
			.targetIndex = 0,
			.pProgram = &slangProgram
		}),
		return cleanupPipelineCreation(creationResources)
	);

	Slang::ComPtr<slang::IBlob>& vertexSpirv = slangProgram.entryPointCodes[0];
	Slang::ComPtr<slang::IBlob>& fragmentSpirv = slangProgram.entryPointCodes[1];

	const size_t vertexSpirvSize = vertexSpirv->getBufferSize();
	const void* vertexSpirvBytes = vertexSpirv->getBufferPointer();
	const size_t fragmentSpirvSize = fragmentSpirv->getBufferSize();
	const void* fragmentSpirvBytes = fragmentSpirv->getBufferPointer();

	VK_TOOLKIT_SPV_REFLECT_RESULT_ERROR(
		spvReflectCreateShaderModule(vertexSpirvSize, vertexSpirvBytes, &vertexSpvModule), return cleanupPipelineCreation(creationResources)
	);
	vertexSpvModuleCreated = 1;

	VK_TOOLKIT_SPV_REFLECT_RESULT_ERROR(
		spvReflectCreateShaderModule(fragmentSpirvSize, fragmentSpirvBytes, &fragmentSpvModule), return cleanupPipelineCreation(creationResources)
	);
	fragmentSpvModuleCreated = 1;

	VkShaderModuleCreateInfo vertexShaderModuleCreateInfo = {
		.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext    = NULL,
		.flags    = 0,
		.codeSize = vertexSpirvSize,
		.pCode    = (uint32_t*)(vertexSpirvBytes)
	};

	VK_TOOLKIT_VK_RESULT_ERROR(
		vkCreateShaderModule(args.device, &vertexShaderModuleCreateInfo, NULL, &vertexVkModule), return cleanupPipelineCreation(creationResources)
	);

	VkShaderModuleCreateInfo fragmentShaderModuleCreateInfo = {
		.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext    = NULL,
		.flags    = 0,
		.codeSize = fragmentSpirvSize,
		.pCode    = (uint32_t*)(fragmentSpirvBytes)
	};

	VK_TOOLKIT_VK_RESULT_ERROR(
		vkCreateShaderModule(args.device, &fragmentShaderModuleCreateInfo, NULL, &fragmentVkModule), return cleanupPipelineCreation(creationResources)
	);

	VkBridgeNamedShaderStageInputInfo vertexAdditionalInputs[] = {
		{
			.name = "input.position",
			.info = (VkBridgeShaderStageInputAdditionalInfo) {
				.binding = 0,
				.offset = 0,
			}
		},
		{
			.name = "input.model[0]",
			.info = (VkBridgeShaderStageInputAdditionalInfo) {
				.binding = 1,
				.offset = 0,
			}
		},
		{
			.name = "input.model[1]",
			.info = (VkBridgeShaderStageInputAdditionalInfo) {
				.binding = 1,
				.offset = 16,
			}
		},
		{
			.name = "input.model[2]",
			.info = (VkBridgeShaderStageInputAdditionalInfo) {
				.binding = 1,
				.offset = 32,
			}
		},
		{
			.name = "input.model[3]",
			.info = (VkBridgeShaderStageInputAdditionalInfo) {
				.binding = 1,
				.offset = 48,
			}
		},
	};

	VkBridgeSpecializationConstantSource specializationConstantSources[] = {
		{
			.name       = "LIGHT_INTENSITY",
			.offset     = 0,
			.size       = sizeof(float)
		}
	};

	float specializationData[1] = {
		1.0f
	};

	VK_TOOLKIT_BRIDGE_CHECK(vkBridgeReflectShaderStage((VkBridgeReflectShaderStageArgs) {
		.shaderStage = VK_SHADER_STAGE_VERTEX_BIT,
		.pSpvModule = &vertexSpvModule,
		.shaderInputsExcludeDecorationFlags = SPV_REFLECT_DECORATION_BUILT_IN,
		.shaderInputAdditionalInfoCount = ARRAY_COUNT(vertexAdditionalInputs),
		.pShaderInputAdditionalInfos = vertexAdditionalInputs,
		.specializationConstantSourceCount = 0,
		.pSpecializationConstantSources = NULL,
		.pSpecializationData = NULL,
		.pReflection = &vertexReflection,
	}), return cleanupPipelineCreation(creationResources));

	VK_TOOLKIT_BRIDGE_CHECK(vkBridgeReflectShaderStage((VkBridgeReflectShaderStageArgs) {
		.shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.pSpvModule = &fragmentSpvModule,
		.shaderInputsExcludeDecorationFlags = 0,
		.shaderInputAdditionalInfoCount = 0,
		.pShaderInputAdditionalInfos = NULL,
		.specializationConstantSourceCount = ARRAY_COUNT(specializationConstantSources),
		.pSpecializationConstantSources = specializationConstantSources,
		.pSpecializationData = specializationData,
		.pReflection = &fragmentReflection,
	}), return cleanupPipelineCreation(creationResources));

	const VkBridgeShaderStageReflection* shaderStageReflections[2] = {
		&vertexReflection,
		&fragmentReflection
	};

	VK_TOOLKIT_BRIDGE_CHECK(vkBridgeMergeShaderStageReflections((VkBridgeMergeShaderStageReflectionsArgs) {
		.shaderStageCount = 2,
		.ppShaderStageReflections = shaderStageReflections,
		.pPipelineReflection = &pipelineReflection
	}), return cleanupPipelineCreation(creationResources));

	VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_STATUS, "Push constant ranges reflected");
	VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_STATUS, "Descriptor sets reflected");

	if (!vkToolkitCreatePipelineLayout((VkToolkitCreatePipelineLayoutArgs) {
		.device = args.device,
		.pReflection = &pipelineReflection,
		.pSetLayouts = &setLayouts,
		.pPipelineLayout = &pipelineLayout
	})) {
		return cleanupPipelineCreation(creationResources);
	}

	VkSpecializationInfo specializationInfo = {};

	VK_TOOLKIT_BRIDGE_CHECK(vkBridgeGetSpecializationInfo((VkBridgeGetSpecializationInfoArgs) {
		.pReflection = &fragmentReflection,
		.pSpecializationInfo = &specializationInfo
	}), return cleanupPipelineCreation(creationResources));

	VkVertexInputBindingDescription vertexBindings[2] = {

		(VkVertexInputBindingDescription) {
			.binding   = 0,
			.stride    = sizeof(Vertex),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		},

		(VkVertexInputBindingDescription) {
			.binding = 1,
			.stride = 64,
			.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE
		}
	};

	VkPipelineVertexInputStateCreateInfo vertexInputState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.vertexBindingDescriptionCount = 2,
		.pVertexBindingDescriptions = vertexBindings,
		.vertexAttributeDescriptionCount = vertexReflection.inputAttributeDescriptionCount,
		.pVertexAttributeDescriptions = vertexReflection.pInputAttributeDescriptions,
	};

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};

	VkPipelineRasterizationStateCreateInfo rasterizationState = {
		.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.pNext                   = NULL,
		.flags                   = 0,
		.depthClampEnable        = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode             = VK_POLYGON_MODE_FILL,
		.cullMode                = VK_CULL_MODE_NONE,
		.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.depthBiasEnable         = VK_FALSE,
		.depthBiasConstantFactor = 0.0f,
		.depthBiasClamp          = 0.0f,
		.depthBiasSlopeFactor    = 0.0f,
		.lineWidth               = 1.0f
	};

	VkPipelineMultisampleStateCreateInfo multisampleState = {
		.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.pNext                 = VK_NULL_HANDLE,
		.flags                 = 0,
		.rasterizationSamples  = args.sampleCount,
		.sampleShadingEnable   = VK_FALSE,
		.minSampleShading      = 0.0f,
		.pSampleMask           = VK_NULL_HANDLE,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable      = VK_FALSE
	};

	VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
		.blendEnable         = VK_TRUE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.colorBlendOp        = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp        = VK_BLEND_OP_ADD,
		.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT |
		                       VK_COLOR_COMPONENT_G_BIT |
		                       VK_COLOR_COMPONENT_B_BIT |
		                       VK_COLOR_COMPONENT_A_BIT
	};

	VkPipelineColorBlendStateCreateInfo colorBlendState = {
		.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext           = VK_NULL_HANDLE,
		.flags           = 0,
		.logicOpEnable   = VK_FALSE,
		.logicOp         = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments    = &colorBlendAttachmentState,
		.blendConstants  = { 0.0f, 0.0f, 0.0f }
	};

	VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = {
		.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext               = NULL,
		.flags               = 0,
		.stage               = VK_SHADER_STAGE_VERTEX_BIT,
		.module              = vertexVkModule,
		.pName               = vertexSpvModule.entry_point_name,
		.pSpecializationInfo = NULL,
	};

	VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = {
		.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext               = NULL,
		.flags               = 0,
		.stage               = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module              = fragmentVkModule,
		.pName               = fragmentSpvModule.entry_point_name,
		.pSpecializationInfo = &specializationInfo,
	};

	VkPipelineShaderStageCreateInfo shaderStages[2] = {
		vertexShaderStageCreateInfo,
		fragmentShaderStageCreateInfo
	};

	VkPipelineDepthStencilStateCreateInfo depthStencilState = {
		.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.pNext                 = NULL,
		.flags                 = 0,
		.depthTestEnable       = VK_TRUE,
		.depthWriteEnable      = VK_TRUE,
		.depthCompareOp        = VK_COMPARE_OP_LESS,
		.depthBoundsTestEnable = VK_FALSE,
		.stencilTestEnable     = VK_FALSE,
		.front                 = {},
		.back                  = {},
		.minDepthBounds        = 0.0f,
		.maxDepthBounds        = 1.0f
	};

	VkPipelineRenderingCreateInfo renderingCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.pNext = NULL,
		.viewMask = 0,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &args.colorFormat,
		.depthAttachmentFormat = args.depthFormat,
		.stencilAttachmentFormat = VK_FORMAT_UNDEFINED
	};

	VkDynamicState dynamicStates[2] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = 2,
		.pDynamicStates = dynamicStates
	};

	VkPipelineViewportStateCreateInfo viewportState = {
		.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pNext         = VK_NULL_HANDLE,
		.flags         = 0,
		.viewportCount = 1,
		.pViewports    = NULL,
		.scissorCount  = 1,
		.pScissors     = NULL
	};

	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
		.sType                = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext                = &renderingCreateInfo,
		.flags                = 0,
		.stageCount           = sizeof(shaderStages) / sizeof(shaderStages[0]),
		.pStages              = shaderStages,
		.pVertexInputState    = &vertexInputState,
		.pInputAssemblyState  = &inputAssemblyState,
		.pTessellationState   = NULL,
		.pViewportState       = &viewportState,
		.pRasterizationState  = &rasterizationState,
		.pMultisampleState    = &multisampleState,
		.pDepthStencilState   = &depthStencilState,
		.pColorBlendState     = &colorBlendState,
		.pDynamicState        = &dynamicState,
		.layout               = pipelineLayout,
		.renderPass           = VK_NULL_HANDLE,
		.subpass              = 0,
		.basePipelineHandle   = VK_NULL_HANDLE,
		.basePipelineIndex    = 0
	};

	VK_TOOLKIT_VK_RESULT_ERROR(
		vkCreateGraphicsPipelines(args.device, 0, 1, &graphicsPipelineCreateInfo, NULL, &graphicsPipeline), return cleanupPipelineCreation(creationResources)
	);

	VK_TOOLKIT_LOG(1, VK_TOOLKIT_APP_NAME, VK_TOOLKIT_LOG_LEVEL_STATUS, "Graphics pipeline created");

	vkDestroyShaderModule(args.device, fragmentVkModule, NULL);
	vkDestroyShaderModule(args.device, vertexVkModule, NULL);
	spvReflectDestroyShaderModule(&fragmentSpvModule);
	spvReflectDestroyShaderModule(&vertexSpvModule);
	fragmentVkModule = VK_NULL_HANDLE;
	vertexVkModule = VK_NULL_HANDLE;
	fragmentSpvModuleCreated = 0;
	vertexSpvModuleCreated = 0;

	args.pPipeline->vertexReflection = vertexReflection;
	args.pPipeline->fragmentReflection = fragmentReflection;
	args.pPipeline->pipelineReflection = pipelineReflection;
	args.pPipeline->setLayouts = std::move(setLayouts);
	args.pPipeline->layout = pipelineLayout;
	args.pPipeline->pipeline = graphicsPipeline;

	return 1;

}

void destroyPipeline(VkDevice device, Pipeline* pPipeline) {
	if (device == VK_NULL_HANDLE || pPipeline == NULL) {
		return;
	}

	vkDestroyPipeline(device, pPipeline->pipeline, NULL);
	vkToolkitDestroyPipelineLayout(device, &pPipeline->setLayouts, &pPipeline->layout);
	vkBridgeDestroyPipelineReflection(&pPipeline->pipelineReflection);
	vkBridgeDestroyShaderStageReflection(&pPipeline->fragmentReflection);
	vkBridgeDestroyShaderStageReflection(&pPipeline->vertexReflection);
	*pPipeline = Pipeline {};
}
