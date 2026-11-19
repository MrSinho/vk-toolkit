#ifndef VK_BRIDGE_H
#define VK_BRIDGE_H

#include <stddef.h>
#include <stdint.h>

#include <vulkan/vulkan.h>
#include <spirv_reflect.h>



#ifdef __cplusplus
extern "C" {
#endif



typedef enum VkBridgeResult {
    VK_BRIDGE_SUCCESS = 0,
    VK_BRIDGE_FAILURE = -1,
    VK_BRIDGE_ERROR_INVALID_ARGUMENT = -2,
    VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY = -3,
    VK_BRIDGE_ERROR_NOT_FOUND = -4,
    VK_BRIDGE_ERROR_UNSUPPORTED = -5,
    VK_BRIDGE_ERROR_BUFFER_TOO_SMALL = -6,
    VK_BRIDGE_ERROR_LIMIT_EXCEEDED = -7,
    VK_BRIDGE_ERROR_INCOMPATIBLE_DATA = -8
} VkBridgeResult;

const char* vkBridgeTranslateResult(VkBridgeResult result);

#define VK_BRIDGE_CHECK(functionCall, failureExpression)\
    do {\
        const VkBridgeResult vkBridgeCheckedResult = (functionCall);\
        if (vkBridgeCheckedResult != VK_BRIDGE_SUCCESS) {\
            failureExpression;\
        }\
    } while (0)


#define VK_BRIDGE_CHECK_SPV_REFLECT(functionCall, failureExpression)\
    do {\
        const SpvReflectResult vkBridgeCheckedSpvResult = (functionCall);\
        if (vkBridgeCheckedSpvResult != SPV_REFLECT_RESULT_SUCCESS) {\
            failureExpression;\
        }\
    } while (0)

const char* vkBridgeTranslateSpvReflectResult(SpvReflectResult spvResult);



typedef struct VkBridgeDescriptorSetLayoutBinding {
    const char* name;
    uint32_t set; // might look redundant, but is useful when merging set layout bindings
    VkDescriptorSetLayoutBinding setLayoutBinding;
} VkBridgeDescriptorSetLayoutBinding;

typedef struct VkBridgeShaderStageDescriptorSet { // VkBridgeDescriptorSet --> multiple set layout bindings, same set
    uint32_t set;
    uint32_t setLayoutBindingCount;
    VkBridgeDescriptorSetLayoutBinding* pSetLayoutBindings;
} VkBridgeShaderStageDescriptorSet;

typedef struct VkBridgeDescriptorSet {
    uint32_t set;
    uint32_t setLayoutBindingCount;
    VkDescriptorSetLayoutBinding* pSetLayoutBindings;
    const char** pSetLayoutBindingNames;
} VkBridgeDescriptorSet;

typedef struct VkBridgeGetShaderStageDescriptorSetsArgs {
    uint32_t setCount;
    SpvReflectDescriptorSet** ppSpvSets;
    VkShaderStageFlags shaderStageFlags;
    VkBridgeShaderStageDescriptorSet** ppStageDescriptorSets;
} VkBridgeGetShaderStageDescriptorSetsArgs;

VkBridgeResult vkBridgeGetShaderStageDescriptorSets(VkBridgeGetShaderStageDescriptorSetsArgs args);

void vkBridgeDestroyShaderStageDescriptorSets(
    uint32_t descriptorSetCount,
    VkBridgeShaderStageDescriptorSet* pDescriptorSets
);

typedef struct VkBridgeFindBindingsArgs {
    uint32_t srcSetLayoutBindingCount;
    const VkBridgeDescriptorSetLayoutBinding* pSrcSetLayoutBindings;

    uint32_t set;
    uint32_t binding;

    uint32_t* pSameBindingCount;
    VkBridgeDescriptorSetLayoutBinding* pSameSetLayoutBindings;
} VkBridgeFindBindingsArgs;

VkBridgeResult vkBridgeFindBindings(VkBridgeFindBindingsArgs args);

VkBridgeResult vkBridgeIsBindingRedundant(
    VkBridgeDescriptorSetLayoutBinding setLayoutBinding,
    uint32_t redundantBindingCount,
    const VkBridgeDescriptorSetLayoutBinding* pRedundantBindings,
    uint8_t* pIsRedundant
);

typedef struct VkBridgeMergeShaderStageDescriptorSetsArgs {
    uint32_t shaderStageCount;

    const uint32_t* pStageSetCounts;
    const VkBridgeShaderStageDescriptorSet* const* ppStageDescriptorSets;

    uint32_t* pDescriptorSetCount;
    VkBridgeDescriptorSet** ppDstDescriptorSets;
} VkBridgeMergeShaderStageDescriptorSetsArgs;

VkBridgeResult vkBridgeMergeShaderStageDescriptorSets(VkBridgeMergeShaderStageDescriptorSetsArgs args);

void vkBridgeDestroyDescriptorSets(
    uint32_t descriptorSetCount,
    VkBridgeDescriptorSet* pDescriptorSets
);

typedef struct VkBridgeGetPoolSizesArgs {
    uint32_t setCount;
    const VkBridgeDescriptorSet* pSets;
    uint32_t descriptorCountMultiplier;
    uint32_t *pMaxSets;
    uint32_t* pPoolSizeCount;
    VkDescriptorPoolSize* pPoolSizes;
} VkBridgeGetPoolSizesArgs;

VkBridgeResult vkBridgeGetPoolSizes(VkBridgeGetPoolSizesArgs args);

typedef struct VkBridgeGetSetLayoutBindingByNameArgs {
    const char* name;
    uint32_t setCount;
    const VkBridgeDescriptorSet* pSets;
    VkBridgeDescriptorSetLayoutBinding* pSetLayoutBinding;
} VkBridgeGetSetLayoutBindingByNameArgs;

VkBridgeResult vkBridgeGetSetLayoutBindingByName(VkBridgeGetSetLayoutBindingByNameArgs args);



typedef VkPushConstantRange VkBridgePushConstantRange;

typedef struct VkBridgeGetShaderStagePushConstantRangesArgs {
    uint32_t rangeCount;
    SpvReflectBlockVariable** ppSpvPushConstants;
    VkShaderStageFlags shaderStage;
    VkBridgePushConstantRange* pDstRanges;
} VkBridgeGetShaderStagePushConstantRangesArgs;

VkBridgeResult vkBridgeGetShaderStagePushConstantRanges(VkBridgeGetShaderStagePushConstantRangesArgs args);


typedef struct VkBridgeMergePushConstantRangesArgs {
    uint32_t rangeCount;
    const VkBridgePushConstantRange* pRanges;

    uint32_t* pMergedRangeCount;
    VkBridgePushConstantRange* pMergedRanges;
} VkBridgeMergePushConstantRangesArgs;


VkBridgeResult vkBridgeMergePushConstantRanges(VkBridgeMergePushConstantRangesArgs args);



typedef struct VkBridgeShaderStageInput {
    const char* name;
    uint32_t location;
    VkFormat format; // Undefined for composite inputs.
    //SpvReflectDecorationFlags decorationFlags;
} VkBridgeShaderStageInput;

typedef struct VkBridgeShaderStageInputAdditionalInfo {
    uint32_t binding;
    uint32_t offset;
} VkBridgeShaderStageInputAdditionalInfo;

typedef struct VkBridgeGetShaderStageInputsArgs {
    SpvReflectShaderModule* pShaderModule;
    SpvReflectDecorationFlags excludeDecorationFlags;
    uint32_t* pInputCount;
    VkBridgeShaderStageInput* pInputs;
} VkBridgeGetShaderStageInputsArgs;

SpvReflectResult vkBridgeGetShaderStageInputs(VkBridgeGetShaderStageInputsArgs args);

void vkBridgeDestroyShaderStageInputs(
    uint32_t inputCount,
    VkBridgeShaderStageInput* pInputs
);



typedef VkSpecializationMapEntry VkBridgeSpecializationMapEntry;

typedef struct VkBridgeSpecializationConstantSource {
    const char* name;
    uint32_t offset;
    uint32_t size;
} VkBridgeSpecializationConstantSource;

typedef struct VkBridgeGetSpecializationMapEntriesArgs {
    SpvReflectShaderModule* pShaderModule;
    uint32_t srcConstantCount;
    const VkBridgeSpecializationConstantSource* pSources;
    uint32_t* pEntryCount;
    VkBridgeSpecializationMapEntry* pEntries;
} VkBridgeGetSpecializationMapEntriesArgs;

SpvReflectResult vkBridgeGetSpecializationMapEntries(VkBridgeGetSpecializationMapEntriesArgs args);



typedef struct VkBridgeShaderStageReflection {
    VkShaderStageFlagBits shaderStage;
    uint32_t pushConstantRangeCount;
    VkBridgePushConstantRange* pPushConstantRanges;
    uint32_t descriptorSetCount;
    VkBridgeShaderStageDescriptorSet* pDescriptorSets;
    uint32_t inputAttributeDescriptionCount;
    VkVertexInputAttributeDescription* pInputAttributeDescriptions;
    uint32_t specializationMapEntryCount;
    VkBridgeSpecializationMapEntry* pSpecializationMapEntries;
    size_t specializationDataSize;
    const void* pSpecializationData;
} VkBridgeShaderStageReflection;

typedef struct VkBridgeNamedShaderStageInputInfo {
    const char* name;
    VkBridgeShaderStageInputAdditionalInfo info;
} VkBridgeNamedShaderStageInputInfo;

typedef struct VkBridgeReflectShaderStageArgs {
    VkShaderStageFlagBits shaderStage;
    SpvReflectShaderModule* pSpvModule;
    SpvReflectDecorationFlags shaderInputsExcludeDecorationFlags;
    uint32_t shaderInputAdditionalInfoCount;
    const VkBridgeNamedShaderStageInputInfo* pShaderInputAdditionalInfos;
    uint32_t specializationConstantSourceCount;
    const VkBridgeSpecializationConstantSource* pSpecializationConstantSources;
    const void* pSpecializationData;
    VkBridgeShaderStageReflection* pReflection;
} VkBridgeReflectShaderStageArgs;

VkBridgeResult vkBridgeReflectShaderStage(VkBridgeReflectShaderStageArgs args);

void vkBridgeDestroyShaderStageReflection(VkBridgeShaderStageReflection* pReflection);

typedef struct VkBridgePipelineReflection {
    uint32_t pushConstantRangeCount;
    VkBridgePushConstantRange* pPushConstantRanges;
    uint32_t descriptorSetCount;
    VkBridgeDescriptorSet* pDescriptorSets;
} VkBridgePipelineReflection;

typedef struct VkBridgeMergeShaderStageReflectionsArgs {
    uint32_t shaderStageCount;
    const VkBridgeShaderStageReflection* const* ppShaderStageReflections;
    VkBridgePipelineReflection* pPipelineReflection;
} VkBridgeMergeShaderStageReflectionsArgs;

VkBridgeResult vkBridgeMergeShaderStageReflections(VkBridgeMergeShaderStageReflectionsArgs args);

void vkBridgeDestroyPipelineReflection(VkBridgePipelineReflection* pReflection);

typedef struct VkBridgeGetDescriptorSetLayoutCreateInfoArgs {
    const VkBridgeDescriptorSet* pDescriptorSet;
    VkDescriptorSetLayoutCreateInfo* pCreateInfo;
} VkBridgeGetDescriptorSetLayoutCreateInfoArgs;

VkBridgeResult vkBridgeGetDescriptorSetLayoutCreateInfo(VkBridgeGetDescriptorSetLayoutCreateInfoArgs args);

typedef struct VkBridgeGetPipelineLayoutCreateInfoArgs {
    const VkBridgePipelineReflection* pReflection;
    uint32_t setLayoutCount;
    const VkDescriptorSetLayout* pSetLayouts;
    VkPipelineLayoutCreateInfo* pCreateInfo;
} VkBridgeGetPipelineLayoutCreateInfoArgs;

VkBridgeResult vkBridgeGetPipelineLayoutCreateInfo(VkBridgeGetPipelineLayoutCreateInfoArgs args);

typedef struct VkBridgeGetSpecializationInfoArgs {
    const VkBridgeShaderStageReflection* pReflection;
    VkSpecializationInfo* pSpecializationInfo;
} VkBridgeGetSpecializationInfoArgs;

VkBridgeResult vkBridgeGetSpecializationInfo(VkBridgeGetSpecializationInfoArgs args);

#ifdef __cplusplus
}
#endif

#endif//VK_BRIDGE_H
