#include "vk-bridge/vk-bridge.h"

#include <new>
#include <stdio.h>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <vector>



const char* vkBridgeTranslateResult(VkBridgeResult result) {
    switch (result) {
        case VK_BRIDGE_SUCCESS:
            return "VK_BRIDGE_SUCCESS";
        case VK_BRIDGE_FAILURE:
            return "VK_BRIDGE_FAILURE";
        case VK_BRIDGE_ERROR_INVALID_ARGUMENT:
            return "VK_BRIDGE_ERROR_INVALID_ARGUMENT";
        case VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY:
            return "VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY";
        case VK_BRIDGE_ERROR_NOT_FOUND:
            return "VK_BRIDGE_ERROR_NOT_FOUND";
        case VK_BRIDGE_ERROR_UNSUPPORTED:
            return "VK_BRIDGE_ERROR_UNSUPPORTED";
        case VK_BRIDGE_ERROR_BUFFER_TOO_SMALL:
            return "VK_BRIDGE_ERROR_BUFFER_TOO_SMALL";
        case VK_BRIDGE_ERROR_LIMIT_EXCEEDED:
            return "VK_BRIDGE_ERROR_LIMIT_EXCEEDED";
        case VK_BRIDGE_ERROR_INCOMPATIBLE_DATA:
            return "VK_BRIDGE_ERROR_INCOMPATIBLE_DATA";
        default:
            return "Unhandled VkBridgeResult";
    }
}

static char* vkBridgeDuplicateName(const char* source) {
    if (source == NULL) {
        source = "";
    }

    const size_t size = strlen(source) + 1;
    char* name = (char*)malloc(size);
    if (name != NULL) {
        memcpy(name, source, size);
    }
    return name;
}

static char* vkBridgeDuplicateIndexedName(const char* source, uint32_t index) {
    if (source == NULL) {
        source = "";
    }

    const int length = snprintf(NULL, 0, "%s[%u]", source, index);
    if (length < 0) {
        return NULL;
    }

    char* name = (char*)malloc((size_t)length + 1);
    if (name != NULL) {
        snprintf(name, (size_t)length + 1, "%s[%u]", source, index);
    }
    return name;
}

typedef struct VkBridgeDescriptorSetStorage {
    uint32_t set;
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings;
    std::vector<const char*> setLayoutBindingNames;
} VkBridgeDescriptorSetStorage;





const char* vkBridgeTranslateSpvReflectResult(SpvReflectResult spvResult) {
    switch (spvResult) {
        case SPV_REFLECT_RESULT_SUCCESS:
            return "SPV_REFLECT_RESULT_SUCCESS";
        case SPV_REFLECT_RESULT_NOT_READY:
            return "SPV_REFLECT_RESULT_NOT_READY";
        case SPV_REFLECT_RESULT_ERROR_PARSE_FAILED:
            return "SPV_REFLECT_RESULT_ERROR_PARSE_FAILED";
        case SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED:
            return "SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED";
        case SPV_REFLECT_RESULT_ERROR_RANGE_EXCEEDED:
            return "SPV_REFLECT_RESULT_ERROR_RANGE_EXCEEDED";
        case SPV_REFLECT_RESULT_ERROR_NULL_POINTER:
            return "SPV_REFLECT_RESULT_ERROR_NULL_POINTER";
        case SPV_REFLECT_RESULT_ERROR_INTERNAL_ERROR:
            return "SPV_REFLECT_RESULT_ERROR_INTERNAL_ERROR";
        case SPV_REFLECT_RESULT_ERROR_COUNT_MISMATCH:
            return "SPV_REFLECT_RESULT_ERROR_COUNT_MISMATCH";
        case SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND:
            return "SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND";
        case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_CODE_SIZE:
            return "SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_CODE_SIZE";
        case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_MAGIC_NUMBER:
            return "SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_MAGIC_NUMBER";
        case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_EOF:
            return "SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_EOF";
        case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ID_REFERENCE:
            return "SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ID_REFERENCE";
        case SPV_REFLECT_RESULT_ERROR_SPIRV_SET_NUMBER_OVERFLOW:
            return "SPV_REFLECT_RESULT_ERROR_SPIRV_SET_NUMBER_OVERFLOW";
        case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_STORAGE_CLASS:
            return "SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_STORAGE_CLASS";
        case SPV_REFLECT_RESULT_ERROR_SPIRV_RECURSION:
            return "SPV_REFLECT_RESULT_ERROR_SPIRV_RECURSION";
        case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_INSTRUCTION:
            return "SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_INSTRUCTION";
        case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_BLOCK_DATA:
            return "SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_BLOCK_DATA";
        case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_BLOCK_MEMBER_REFERENCE:
            return "SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_BLOCK_MEMBER_REFERENCE";
        case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ENTRY_POINT:
            return "SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ENTRY_POINT";
        case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_EXECUTION_MODE:
            return "SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_EXECUTION_MODE";
        case SPV_REFLECT_RESULT_ERROR_SPIRV_MAX_RECURSIVE_EXCEEDED:
            return "SPV_REFLECT_RESULT_ERROR_SPIRV_MAX_RECURSIVE_EXCEEDED";
        default:
            return "Unhandled SPV Reflect result";
    };
}



VkBridgeResult vkBridgeGetShaderStageDescriptorSets(VkBridgeGetShaderStageDescriptorSetsArgs args) {
    if (args.ppStageDescriptorSets == NULL || (args.setCount > 0 && args.ppSpvSets == NULL)) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    *args.ppStageDescriptorSets = NULL;
    VkBridgeShaderStageDescriptorSet* stageDescriptorSets = NULL;

    try {
        stageDescriptorSets = (VkBridgeShaderStageDescriptorSet*)calloc(args.setCount, sizeof(VkBridgeShaderStageDescriptorSet));
        if (args.setCount > 0 && stageDescriptorSets == NULL) {
            return VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY;
        }

        for (uint32_t i = 0; i < args.setCount; i++) {
            SpvReflectDescriptorSet* pSpvSet = args.ppSpvSets[i];
            if (pSpvSet == NULL || (pSpvSet->binding_count > 0 && pSpvSet->bindings == NULL)) {
                vkBridgeDestroyShaderStageDescriptorSets(args.setCount, stageDescriptorSets);
                return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
            }

            VkBridgeShaderStageDescriptorSet* pStageDescriptorSet = &stageDescriptorSets[i];
            pStageDescriptorSet->set = pSpvSet->set;
            pStageDescriptorSet->setLayoutBindingCount = pSpvSet->binding_count;
            pStageDescriptorSet->pSetLayoutBindings = (VkBridgeDescriptorSetLayoutBinding*)calloc(
                pSpvSet->binding_count,
                sizeof(VkBridgeDescriptorSetLayoutBinding)
            );
            if (pSpvSet->binding_count > 0 && pStageDescriptorSet->pSetLayoutBindings == NULL) {
                vkBridgeDestroyShaderStageDescriptorSets(args.setCount, stageDescriptorSets);
                return VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY;
            }

            for (uint32_t j = 0; j < pSpvSet->binding_count; j++) {
                SpvReflectDescriptorBinding* spvSetBinding = pSpvSet->bindings[j];
                if (spvSetBinding == NULL) {
                    vkBridgeDestroyShaderStageDescriptorSets(args.setCount, stageDescriptorSets);
                    return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
                }

                VkBridgeDescriptorSetLayoutBinding setLayoutBinding = {};
                setLayoutBinding.name = vkBridgeDuplicateName(spvSetBinding->name);
                if (setLayoutBinding.name == NULL) {
                    vkBridgeDestroyShaderStageDescriptorSets(args.setCount, stageDescriptorSets);
                    return VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY;
                }
                setLayoutBinding.set = pSpvSet->set;
                setLayoutBinding.setLayoutBinding = {
                    .binding            = spvSetBinding->binding,
                    .descriptorType     = (VkDescriptorType)spvSetBinding->descriptor_type,
                    .descriptorCount    = spvSetBinding->count,
                    .stageFlags         = args.shaderStageFlags,
                    .pImmutableSamplers = NULL
                };
                pStageDescriptorSet->pSetLayoutBindings[j] = setLayoutBinding;
            }
        }

        *args.ppStageDescriptorSets = stageDescriptorSets;
    }
    catch (const std::bad_alloc&) {
        vkBridgeDestroyShaderStageDescriptorSets(args.setCount, stageDescriptorSets);
        return VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY;
    }
    catch (const std::length_error&) {
        vkBridgeDestroyShaderStageDescriptorSets(args.setCount, stageDescriptorSets);
        return VK_BRIDGE_ERROR_LIMIT_EXCEEDED;
    }

    return VK_BRIDGE_SUCCESS;
}

void vkBridgeDestroyShaderStageDescriptorSets(
    uint32_t descriptorSetCount,
    VkBridgeShaderStageDescriptorSet* pDescriptorSets
) {
    if (pDescriptorSets == NULL) {
        return;
    }

    for (uint32_t i = 0; i < descriptorSetCount; i++) {
        if (pDescriptorSets[i].pSetLayoutBindings != NULL) {
            for (uint32_t j = 0; j < pDescriptorSets[i].setLayoutBindingCount; j++) {
                free((void*)pDescriptorSets[i].pSetLayoutBindings[j].name);
            }
        }
        free(pDescriptorSets[i].pSetLayoutBindings);
    }

    free(pDescriptorSets);
}

VkBridgeResult vkBridgeFindBindings(VkBridgeFindBindingsArgs args) {
    if (args.srcSetLayoutBindingCount > 0 && args.pSrcSetLayoutBindings == NULL) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    uint32_t sameBindingCount = 0;

    for (uint32_t i = 0; i < args.srcSetLayoutBindingCount; i++) {
        const VkBridgeDescriptorSetLayoutBinding* pBinding = &args.pSrcSetLayoutBindings[i];

        if (pBinding->set != args.set || pBinding->setLayoutBinding.binding != args.binding) {
            continue;
        }

        if (args.pSameSetLayoutBindings != NULL) {
            args.pSameSetLayoutBindings[sameBindingCount] = *pBinding;
        }

        sameBindingCount++;
    }

    if (args.pSameBindingCount != NULL) {
        *args.pSameBindingCount = sameBindingCount;
    }

    return VK_BRIDGE_SUCCESS;
}

VkBridgeResult vkBridgeIsBindingRedundant(
    VkBridgeDescriptorSetLayoutBinding setLayoutBinding,
    uint32_t redundantBindingCount,
    const VkBridgeDescriptorSetLayoutBinding* pRedundantBindings,
    uint8_t* pIsRedundant
) {
    if (pIsRedundant == NULL || (redundantBindingCount > 0 && pRedundantBindings == NULL)) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    *pIsRedundant = 0;

    for (uint32_t i = 0; i < redundantBindingCount; i++) {
        const VkBridgeDescriptorSetLayoutBinding* pRedundantBinding = &pRedundantBindings[i];

        if (pRedundantBinding->set == setLayoutBinding.set && pRedundantBinding->setLayoutBinding.binding == setLayoutBinding.setLayoutBinding.binding) {
            *pIsRedundant = 1;
            break;
        }
    }

    return VK_BRIDGE_SUCCESS;
}

VkBridgeResult vkBridgeMergeShaderStageDescriptorSets(VkBridgeMergeShaderStageDescriptorSetsArgs args) {
    if (args.pDescriptorSetCount == NULL) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }
    if (args.shaderStageCount > 0 && (args.pStageSetCounts == NULL || args.ppStageDescriptorSets == NULL)) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    if (args.ppDstDescriptorSets != NULL) {
        *args.ppDstDescriptorSets = NULL;
    }

    try {
        std::vector<VkBridgeDescriptorSetLayoutBinding> allStageBindings;

    // 1. Flatten all shader stages into one array

    for (uint32_t stageIdx = 0; stageIdx < args.shaderStageCount; stageIdx++) {
        uint32_t stageSetCount = args.pStageSetCounts[stageIdx];

        const VkBridgeShaderStageDescriptorSet* pStageDescriptorSets = args.ppStageDescriptorSets[stageIdx];
        if (stageSetCount > 0 && pStageDescriptorSets == NULL) {
            return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
        }

        for (uint32_t setIdx = 0; setIdx < stageSetCount; setIdx++) {
            const VkBridgeShaderStageDescriptorSet* pStageDescriptorSet = &pStageDescriptorSets[setIdx];

            if (pStageDescriptorSet->setLayoutBindingCount > 0 && pStageDescriptorSet->pSetLayoutBindings == NULL) {
                return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
            }

            for (uint32_t bindingIdx = 0; bindingIdx < pStageDescriptorSet->setLayoutBindingCount; bindingIdx++) {
                allStageBindings.push_back(pStageDescriptorSet->pSetLayoutBindings[bindingIdx]);
            }
        }
    }


    // 2. Produce one binding for every unique (set,binding)

    std::vector<VkBridgeDescriptorSetLayoutBinding> mergedBindings;

    for (uint32_t currentBindingIdx = 0; currentBindingIdx < allStageBindings.size(); currentBindingIdx++) {
        VkBridgeDescriptorSetLayoutBinding* pCurrentBinding = &allStageBindings[currentBindingIdx];
        uint32_t sameBindingCount = 0;

        VkBridgeFindBindingsArgs findArgs = {
            .srcSetLayoutBindingCount = (uint32_t)allStageBindings.size(),
            .pSrcSetLayoutBindings    = allStageBindings.data(),
            .set                      = pCurrentBinding->set,
            .binding                  = pCurrentBinding->setLayoutBinding.binding,
            .pSameBindingCount        = &sameBindingCount,
            .pSameSetLayoutBindings   = NULL
        };

        VkBridgeResult result = vkBridgeFindBindings(findArgs);
        if (result != VK_BRIDGE_SUCCESS) {
            return result;
        }

        // If this (set,binding) has already been emitted, skip it.

        uint8_t alreadyMerged = 0;

        for (uint32_t mergedBindingIdx = 0; mergedBindingIdx < mergedBindings.size(); mergedBindingIdx++) {
            const VkBridgeDescriptorSetLayoutBinding* pMergedBinding = &mergedBindings[mergedBindingIdx];
            if (pMergedBinding->set == pCurrentBinding->set && pMergedBinding->setLayoutBinding.binding == pCurrentBinding->setLayoutBinding.binding) {
                alreadyMerged = 1;
                break;
            }
        }

        if (alreadyMerged) {
            continue;
        }

        std::vector<VkBridgeDescriptorSetLayoutBinding> sameBindings(sameBindingCount);
        findArgs.pSameSetLayoutBindings = sameBindings.data();

        result = vkBridgeFindBindings(findArgs);
        if (result != VK_BRIDGE_SUCCESS) {
            return result;
        }

        // Start with the first occurrence

        VkBridgeDescriptorSetLayoutBinding mergedBinding = *pCurrentBinding;


        // Merge all shader-stage occurrences

        for (uint32_t sameBindingIdx = 0; sameBindingIdx < sameBindings.size(); sameBindingIdx++) {
            const VkBridgeDescriptorSetLayoutBinding* pSameBinding = &sameBindings[sameBindingIdx];

            // Conflicting types shouldn't appear if a composite slang shader program has been generated, it wouldn't assign conflicting descriptor types to the same set and binding
            if (pSameBinding->setLayoutBinding.descriptorType != mergedBinding.setLayoutBinding.descriptorType) {
                return VK_BRIDGE_ERROR_INCOMPATIBLE_DATA;
            }

            if (pSameBinding->setLayoutBinding.descriptorCount != mergedBinding.setLayoutBinding.descriptorCount) {
                return VK_BRIDGE_ERROR_INCOMPATIBLE_DATA;
            }

            mergedBinding.setLayoutBinding.stageFlags |= pSameBinding->setLayoutBinding.stageFlags;
        }

        mergedBindings.push_back(mergedBinding);
    }


    // 3. Regroup the flat bindings into DescriptorSets

    std::vector<VkBridgeDescriptorSetStorage> descriptorSets;

    for (uint32_t mergedBindingIdx = 0; mergedBindingIdx < mergedBindings.size(); mergedBindingIdx++) {
        const VkBridgeDescriptorSetLayoutBinding* pMergedBinding = &mergedBindings[mergedBindingIdx];

        if (descriptorSets.size() <= pMergedBinding->set) {
            const uint32_t previousSetCount = (uint32_t)descriptorSets.size();
            descriptorSets.resize((size_t)pMergedBinding->set + 1);
            for (uint32_t setIdx = previousSetCount; setIdx < descriptorSets.size(); setIdx++) {
                descriptorSets[setIdx].set = setIdx;
            }
        }

        VkBridgeDescriptorSetStorage* pDescriptorSet = &descriptorSets[pMergedBinding->set];
        pDescriptorSet->setLayoutBindings.push_back(pMergedBinding->setLayoutBinding);
        pDescriptorSet->setLayoutBindingNames.push_back(pMergedBinding->name);
    }

    // 4. Return result

    *args.pDescriptorSetCount = (uint32_t)descriptorSets.size();

    if (args.ppDstDescriptorSets != NULL) {
        VkBridgeDescriptorSet* dstDescriptorSets = (VkBridgeDescriptorSet*)calloc(
            descriptorSets.size(),
            sizeof(VkBridgeDescriptorSet)
        );
        if (!descriptorSets.empty() && dstDescriptorSets == NULL) {
            return VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY;
        }

        for (uint32_t i = 0; i < descriptorSets.size(); i++) {
            const VkBridgeDescriptorSetStorage* pSrcDescriptorSet = &descriptorSets[i];
            VkBridgeDescriptorSet* pDstDescriptorSet = &dstDescriptorSets[i];

            pDstDescriptorSet->set = pSrcDescriptorSet->set;
            pDstDescriptorSet->setLayoutBindingCount = (uint32_t)pSrcDescriptorSet->setLayoutBindings.size();
            pDstDescriptorSet->pSetLayoutBindings = (VkDescriptorSetLayoutBinding*)calloc(
                pDstDescriptorSet->setLayoutBindingCount,
                sizeof(VkDescriptorSetLayoutBinding)
            );
            pDstDescriptorSet->pSetLayoutBindingNames = (const char**)calloc(
                pDstDescriptorSet->setLayoutBindingCount,
                sizeof(const char*)
            );
            if (pDstDescriptorSet->setLayoutBindingCount > 0 &&
                (pDstDescriptorSet->pSetLayoutBindings == NULL || pDstDescriptorSet->pSetLayoutBindingNames == NULL)) {
                vkBridgeDestroyDescriptorSets((uint32_t)descriptorSets.size(), dstDescriptorSets);
                return VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY;
            }

            for (uint32_t bindingIdx = 0; bindingIdx < pDstDescriptorSet->setLayoutBindingCount; bindingIdx++) {
                pDstDescriptorSet->pSetLayoutBindings[bindingIdx] = pSrcDescriptorSet->setLayoutBindings[bindingIdx];
                pDstDescriptorSet->pSetLayoutBindingNames[bindingIdx] = vkBridgeDuplicateName(
                    pSrcDescriptorSet->setLayoutBindingNames[bindingIdx]
                );
                if (pDstDescriptorSet->pSetLayoutBindingNames[bindingIdx] == NULL) {
                    vkBridgeDestroyDescriptorSets((uint32_t)descriptorSets.size(), dstDescriptorSets);
                    return VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY;
                }
            }
        }

        *args.ppDstDescriptorSets = dstDescriptorSets;
    }

        return VK_BRIDGE_SUCCESS;
    }
    catch (const std::bad_alloc&) {
        return VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY;
    }
    catch (const std::length_error&) {
        return VK_BRIDGE_ERROR_LIMIT_EXCEEDED;
    }
}

void vkBridgeDestroyDescriptorSets(
    uint32_t descriptorSetCount,
    VkBridgeDescriptorSet* pDescriptorSets
) {
    if (pDescriptorSets == NULL) {
        return;
    }

    for (uint32_t i = 0; i < descriptorSetCount; i++) {
        free(pDescriptorSets[i].pSetLayoutBindings);
        if (pDescriptorSets[i].pSetLayoutBindingNames != NULL) {
            for (uint32_t j = 0; j < pDescriptorSets[i].setLayoutBindingCount; j++) {
                free((void*)pDescriptorSets[i].pSetLayoutBindingNames[j]);
            }
        }
        free(pDescriptorSets[i].pSetLayoutBindingNames);
    }

    free(pDescriptorSets);
}

VkBridgeResult vkBridgeGetPoolSizes(VkBridgeGetPoolSizesArgs args) {
    if (args.setCount > 0 && args.pSets == NULL) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }
    if (args.descriptorCountMultiplier == 0) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    try {
        std::vector<VkDescriptorPoolSize> poolSizes;

        for (uint32_t setIdx = 0; setIdx < args.setCount; setIdx++) {

            const VkBridgeDescriptorSet* pSet = &args.pSets[setIdx];

            if (pSet->setLayoutBindingCount > 0 && pSet->pSetLayoutBindings == NULL) {
                return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
            }

            for (uint32_t bindingIdx = 0; bindingIdx < pSet->setLayoutBindingCount; bindingIdx++) {
                const VkDescriptorSetLayoutBinding* pBinding = &pSet->pSetLayoutBindings[bindingIdx];

                const uint32_t descriptorCount = pBinding->descriptorCount * args.descriptorCountMultiplier;
                uint8_t typeFound = 0;
                for (uint32_t poolSizeIdx = 0; poolSizeIdx < poolSizes.size(); poolSizeIdx++) {
                    VkDescriptorPoolSize* pPoolSize = &poolSizes[poolSizeIdx];

                    if (pPoolSize->type == pBinding->descriptorType) {
                        pPoolSize->descriptorCount += descriptorCount;
                        typeFound = 1;
                        break;
                    }
                }

                if (!typeFound) {
                    VkDescriptorPoolSize poolSize = {
                        .type = pBinding->descriptorType,
                        .descriptorCount = descriptorCount
                    };

                    poolSizes.push_back(poolSize);
                }
            }
        }

        if (args.pMaxSets != NULL) {
            (*args.pMaxSets) = args.setCount * args.descriptorCountMultiplier;
        }

        if (args.pPoolSizeCount != NULL) {
            (*args.pPoolSizeCount) = (uint32_t)poolSizes.size();
        }

        if (args.pPoolSizes != NULL) {
            memcpy(args.pPoolSizes, poolSizes.data(), poolSizes.size() * sizeof(poolSizes[0]));
        }

        return VK_BRIDGE_SUCCESS;
    }
    catch (const std::bad_alloc&) {
        return VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY;
    }
    catch (const std::length_error&) {
        return VK_BRIDGE_ERROR_LIMIT_EXCEEDED;
    }
}

VkBridgeResult vkBridgeGetSetLayoutBindingByName(VkBridgeGetSetLayoutBindingByNameArgs args) {
    if (args.name == NULL || (args.setCount > 0 && args.pSets == NULL)) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    for (uint32_t setIdx = 0; setIdx < args.setCount; setIdx++) {

        const VkBridgeDescriptorSet* pSet = &args.pSets[setIdx];
        if (pSet->setLayoutBindingCount > 0 &&
            (pSet->pSetLayoutBindings == NULL || pSet->pSetLayoutBindingNames == NULL)) {
            return VK_BRIDGE_ERROR_INCOMPATIBLE_DATA;
        }

        for (uint32_t setLayoutBindingIdx = 0; setLayoutBindingIdx < pSet->setLayoutBindingCount; setLayoutBindingIdx++) {

            const char* setLayoutBindingName = pSet->pSetLayoutBindingNames[setLayoutBindingIdx];

            if (strcmp(args.name, setLayoutBindingName) == 0) {

                if (args.pSetLayoutBinding != NULL) {

                    VkBridgeDescriptorSetLayoutBinding setLayoutBinding = {
                        .name = setLayoutBindingName,
                        .set = pSet->set,
                        .setLayoutBinding = pSet->pSetLayoutBindings[setLayoutBindingIdx]
                    };

                    (*args.pSetLayoutBinding) = setLayoutBinding;
                }

                return VK_BRIDGE_SUCCESS;
            }

        }

    }

    return VK_BRIDGE_ERROR_NOT_FOUND;
}




VkBridgeResult vkBridgeGetShaderStagePushConstantRanges(VkBridgeGetShaderStagePushConstantRangesArgs args) {
    if (args.rangeCount > 0 && (args.ppSpvPushConstants == NULL || args.pDstRanges == NULL)) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    for (uint32_t i = 0; i < args.rangeCount; i++) {

        SpvReflectBlockVariable* spvPushConstant = args.ppSpvPushConstants[i];
        if (spvPushConstant == NULL) {
            return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
        }

        VkPushConstantRange range = {
            .stageFlags = args.shaderStage,
            .offset     = spvPushConstant->offset,
            .size       = spvPushConstant->size
        };

        args.pDstRanges[i] = range;
    }

    return VK_BRIDGE_SUCCESS;
}

static int vkBridgeCompareUint32(const void* a, const void* b) {
    uint32_t lhs = *(const uint32_t*)a;
    uint32_t rhs = *(const uint32_t*)b;
    return (lhs > rhs) - (lhs < rhs);
}

VkBridgeResult vkBridgeMergePushConstantRanges(VkBridgeMergePushConstantRangesArgs args) {

    if (args.pMergedRangeCount == NULL) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    const uint32_t count = args.rangeCount;
    const VkBridgePushConstantRange* ranges = args.pRanges;

    if (count == 0) {
        *args.pMergedRangeCount = 0;
        return VK_BRIDGE_SUCCESS;
    }

    if (ranges == NULL) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    uint32_t* boundaries = (uint32_t*)calloc(count * 2, sizeof(uint32_t));

    if (!boundaries) {
        return VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < count; i++) {
        boundaries[i * 2 + 0] = ranges[i].offset;
        boundaries[i * 2 + 1] = ranges[i].offset + ranges[i].size;
    }

    qsort(boundaries, count * 2, sizeof(uint32_t), vkBridgeCompareUint32);

    uint32_t boundaryCount = 0;

    for (uint32_t i = 0; i < count * 2; i++) {
        if (boundaryCount == 0 || boundaries[i] != boundaries[boundaryCount - 1]) {
            boundaries[boundaryCount++] = boundaries[i];
        }
    }

    uint32_t mergedCount = 0;

    /*
     * Keep the previous merged range independently of
     * pMergedRanges so that the function works both with
     * and without an output buffer.
     */
    VkPushConstantRange previous = {0};
    uint8_t hasPrevious = 0;

    for (uint32_t i = 0; i + 1 < boundaryCount; i++) {

        const uint32_t begin = boundaries[i];
        const uint32_t end   = boundaries[i + 1];

        VkShaderStageFlags stageFlags = 0;

        for (uint32_t j = 0; j < count; ++j) {

            const VkPushConstantRange* range = &ranges[j];

            const uint32_t rangeBegin = range->offset;
            const uint32_t rangeEnd = range->offset + range->size;

            if (rangeBegin <= begin && rangeEnd >= end) {
                stageFlags |= range->stageFlags;
            }
        }

        if (stageFlags == 0)
            continue;

        /*
         * Merge with the previous interval if possible.
         */
        if (hasPrevious && previous.offset + previous.size == begin && previous.stageFlags == stageFlags) {

            previous.size = end - previous.offset;

            if (args.pMergedRanges != NULL) {
                args.pMergedRanges[mergedCount - 1] = previous;
            }

            continue;
        }

        /*
         * Start a new merged range.
         */
        previous = (VkPushConstantRange) {
            .stageFlags = stageFlags,
            .offset     = begin,
            .size       = end - begin
        };

        hasPrevious = 1;
        mergedCount++;

        if (args.pMergedRanges != NULL) {
            args.pMergedRanges[mergedCount - 1] = previous;
        }
    }

    free(boundaries);

    *args.pMergedRangeCount = mergedCount;

    return VK_BRIDGE_SUCCESS;
}






static SpvReflectResult vkBridgeAppendAllShaderVariableMembers(
    SpvReflectInterfaceVariable* variable,
    const char* parentName,
    uint32_t startLocation,
    SpvReflectDecorationFlags excludeDecorationFlags,
    std::vector<VkBridgeShaderStageInput>* pInputs,
    std::vector<char*>* pInputNames
) {

    if (variable == NULL) {
        return SPV_REFLECT_RESULT_ERROR_NULL_POINTER;
    }

    SpvReflectArrayTraits* array = &variable->array;
    uint32_t currentLocation = startLocation;
    const char* variableName = variable->name != NULL ? variable->name : parentName;

    // Case it's just a simple variable
    if (variable->member_count == 0 && array->dims_count == 0) {

        char* inputName = vkBridgeDuplicateName(variableName);
        if (inputName == NULL) {
            return SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED;
        }

        VkBridgeShaderStageInput input = {
            .name = inputName,
            .location = currentLocation + variable->location,
            .format = (VkFormat)variable->format,
        };

        try {
            pInputNames->push_back(inputName);
        }
        catch (...) {
            free(inputName);
            throw;
        }
        pInputs->push_back(input);

        return SPV_REFLECT_RESULT_SUCCESS;
    }

    // TODO
    // Check the type first to determine the locations occupied by each element
    //SpvReflectTypeDescription* type = variable->type_description;
    //uint32_t locationsPerDimensionElement = 0;
    //type->type_flags;
    //SPV_REFLECT_TYPE_FLAG_VOID                           ; // Don't care
    //SPV_REFLECT_TYPE_FLAG_BOOL                           ; // 1 location
    //SPV_REFLECT_TYPE_FLAG_INT                            ; // 1 location
    //SPV_REFLECT_TYPE_FLAG_FLOAT                          ; // 1 location
    //SPV_REFLECT_TYPE_FLAG_VECTOR                         ; // 1 location
    //SPV_REFLECT_TYPE_FLAG_MATRIX                         ; // columns
    //SPV_REFLECT_TYPE_FLAG_EXTERNAL_IMAGE                 ; // Don't care
    //SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLER               ; // Don't care
    //SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLED_IMAGE         ; // Don't care
    //SPV_REFLECT_TYPE_FLAG_EXTERNAL_BLOCK                 ; // Don't care
    //SPV_REFLECT_TYPE_FLAG_EXTERNAL_ACCELERATION_STRUCTURE; // Don't care
    //SPV_REFLECT_TYPE_FLAG_EXTERNAL_MASK                  ; // Don't care
    //SPV_REFLECT_TYPE_FLAG_STRUCT                         ; // depends
    //SPV_REFLECT_TYPE_FLAG_ARRAY                          ; // depends
    //SPV_REFLECT_TYPE_FLAG_REF                            ; // Don't care
    //type->traits.numeric;
    //type->traits.array;
    //type->struct_type_description;

    // Case it's an array
    uint32_t dimensionCount = array->dims_count;

    for (uint32_t arrayDimIdx = 0; arrayDimIdx < dimensionCount; arrayDimIdx++) {

        uint32_t dimensionLength = array->dims[arrayDimIdx];

        // An array does not consume arrayLength locations. It consumes arrayLength * dimensionLength * locationsPerDimensionElement = for a mat4: 1 * 4 * 1 = 4 locations
        // TODO: Right now the code supposes that each dimension element takes exactly one location, but it could be another array or a struct, so good luck

        for (uint32_t i = 0; i < dimensionLength; i++) {

            char* inputName = vkBridgeDuplicateIndexedName(variableName, i);
            if (inputName == NULL) {
                return SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED;
            }

            VkBridgeShaderStageInput input = {
                .name = inputName,
                .location = currentLocation + variable->location,
                .format = (VkFormat)variable->format, // All elements of an array have the same format, so this is safe
            };

            try {
                pInputNames->push_back(inputName);
            }
            catch (...) {
                free(inputName);
                throw;
            }
            pInputs->push_back(input);
            currentLocation++;
        }


    }


    // Case there are multiple members
    for (uint32_t memberIdx = 0; memberIdx < variable->member_count; memberIdx++) {

        SpvReflectInterfaceVariable* member = &variable->members[memberIdx];

        if (member->decoration_flags & excludeDecorationFlags) {
            continue;
        }

        const SpvReflectResult result = vkBridgeAppendAllShaderVariableMembers(
            member,
            variableName,
            currentLocation,
            excludeDecorationFlags,
            pInputs,
            pInputNames
        );
        if (result != SPV_REFLECT_RESULT_SUCCESS) {
            return result;
        }

    }

    return SPV_REFLECT_RESULT_SUCCESS;
}

SpvReflectResult vkBridgeGetShaderStageInputs(VkBridgeGetShaderStageInputsArgs args) {
    if (args.pShaderModule == NULL || args.pInputCount == NULL) {
        return SPV_REFLECT_RESULT_ERROR_NULL_POINTER;
    }

    uint32_t inputVariableCount = 0;

    SpvReflectResult result = spvReflectEnumerateInputVariables(args.pShaderModule, &inputVariableCount, NULL);
    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        return result;
    }

    std::vector<char*> flattenedInputNames;

    try {
        std::vector<SpvReflectInterfaceVariable*> inputVariables(inputVariableCount);

        result = spvReflectEnumerateInputVariables(args.pShaderModule, &inputVariableCount, inputVariables.data());
        if (result != SPV_REFLECT_RESULT_SUCCESS) {
            return result;
        }

        std::vector<VkBridgeShaderStageInput> flattenedInputs;

        for (uint32_t inputVariableIdx = 0; inputVariableIdx < inputVariables.size(); inputVariableIdx++) {
            SpvReflectInterfaceVariable* variable = inputVariables[inputVariableIdx];
            if (variable == NULL) {
                for (uint32_t i = 0; i < flattenedInputNames.size(); i++) {
                    free(flattenedInputNames[i]);
                }
                return SPV_REFLECT_RESULT_ERROR_NULL_POINTER;
            }

            if (variable->decoration_flags & args.excludeDecorationFlags) {
                continue;
            }

            result = vkBridgeAppendAllShaderVariableMembers(
                variable,
                NULL,
                variable->location,
                args.excludeDecorationFlags,
                &flattenedInputs,
                &flattenedInputNames
            );
            if (result != SPV_REFLECT_RESULT_SUCCESS) {
                for (uint32_t i = 0; i < flattenedInputNames.size(); i++) {
                    free(flattenedInputNames[i]);
                }
                return result;
            }
        }

        *args.pInputCount = (uint32_t)flattenedInputs.size();

        if (args.pInputs != NULL) {
            memcpy(args.pInputs, flattenedInputs.data(), flattenedInputs.size() * sizeof(flattenedInputs[0]));
            flattenedInputNames.clear();
        }
    }
    catch (const std::bad_alloc&) {
        for (uint32_t i = 0; i < flattenedInputNames.size(); i++) {
            free(flattenedInputNames[i]);
        }
        return SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED;
    }
    catch (const std::length_error&) {
        for (uint32_t i = 0; i < flattenedInputNames.size(); i++) {
            free(flattenedInputNames[i]);
        }
        return SPV_REFLECT_RESULT_ERROR_RANGE_EXCEEDED;
    }

    for (uint32_t i = 0; i < flattenedInputNames.size(); i++) {
        free(flattenedInputNames[i]);
    }

    return SPV_REFLECT_RESULT_SUCCESS;
}

void vkBridgeDestroyShaderStageInputs(
    uint32_t inputCount,
    VkBridgeShaderStageInput* pInputs
) {
    if (pInputs == NULL) {
        return;
    }

    for (uint32_t i = 0; i < inputCount; i++) {
        free((void*)pInputs[i].name);
        pInputs[i].name = NULL;
    }
}





SpvReflectResult vkBridgeGetSpecializationMapEntries(VkBridgeGetSpecializationMapEntriesArgs args) {
    if (args.pShaderModule == NULL || args.pEntryCount == NULL ||
        (args.srcConstantCount > 0 && args.pSources == NULL)) {
        return SPV_REFLECT_RESULT_ERROR_NULL_POINTER;
    }

    for (uint32_t srcIdx = 0; srcIdx < args.srcConstantCount; srcIdx++) {
        if (args.pSources[srcIdx].name == NULL) {
            return SPV_REFLECT_RESULT_ERROR_NULL_POINTER;
        }
    }

    uint32_t specializationConstantCount = 0;
    SpvReflectResult result = spvReflectEnumerateSpecializationConstants(args.pShaderModule, &specializationConstantCount, NULL);
    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        return result;
    }

    try {
        std::vector<SpvReflectSpecializationConstant*> specializationConstants(specializationConstantCount);

        result = spvReflectEnumerateSpecializationConstants(args.pShaderModule, &specializationConstantCount, specializationConstants.data());
        if (result != SPV_REFLECT_RESULT_SUCCESS) {
            return result;
        }

        for (uint32_t srcIdx = 0; srcIdx < args.srcConstantCount; srcIdx++) {
            SpvReflectSpecializationConstant* matchingConstant = NULL;

            for (uint32_t constantIdx = 0; constantIdx < specializationConstants.size(); constantIdx++) {
                SpvReflectSpecializationConstant* constant = specializationConstants[constantIdx];
                if (constant == NULL || constant->name == NULL || strcmp(constant->name, args.pSources[srcIdx].name) != 0) {
                    continue;
                }

                matchingConstant = constant;
                break;
            }

            if (matchingConstant == NULL) {
                return SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND;
            }

            if (args.pEntries != NULL) {
                args.pEntries[srcIdx] = (VkBridgeSpecializationMapEntry) {
                    .constantID = matchingConstant->constant_id,
                    .offset = args.pSources[srcIdx].offset,
                    .size = args.pSources[srcIdx].size
                };
            }
        }

        *args.pEntryCount = args.srcConstantCount;
    }
    catch (const std::bad_alloc&) {
        return SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED;
    }
    catch (const std::length_error&) {
        return SPV_REFLECT_RESULT_ERROR_RANGE_EXCEEDED;
    }

    return SPV_REFLECT_RESULT_SUCCESS;
}

VkBridgeResult vkBridgeReflectShaderStage(VkBridgeReflectShaderStageArgs args) {
    if (args.pSpvModule == NULL || args.pReflection == NULL ||
        (args.shaderInputAdditionalInfoCount > 0 && args.pShaderInputAdditionalInfos == NULL) ||
        (args.specializationConstantSourceCount > 0 && args.pSpecializationConstantSources == NULL)) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    (*args.pReflection) = {};

    VkBridgeShaderStageReflection reflection = {
        .shaderStage = args.shaderStage,
        .pSpecializationData = args.pSpecializationData
    };

    std::vector<VkBridgePushConstantRange> reflectRanges;
    std::vector<VkVertexInputAttributeDescription> reflectInputAttributeDescriptions;
    std::vector<VkBridgeSpecializationMapEntry> specializationMapEntries;
    std::vector<VkBridgeShaderStageInput> inputs;

    try {
        uint32_t rangeCount = 0;
        SpvReflectResult reflectResult = spvReflectEnumeratePushConstantBlocks(args.pSpvModule, &rangeCount, NULL);
        if (reflectResult != SPV_REFLECT_RESULT_SUCCESS) {
            return VK_BRIDGE_FAILURE;
        }

        std::vector<SpvReflectBlockVariable*> pushConstantRanges(rangeCount);
        reflectResult = spvReflectEnumeratePushConstantBlocks(args.pSpvModule, &rangeCount, pushConstantRanges.data());
        if (reflectResult != SPV_REFLECT_RESULT_SUCCESS) {
            return VK_BRIDGE_FAILURE;
        }

        reflectRanges.resize(rangeCount);

        VkBridgeGetShaderStagePushConstantRangesArgs getRangesArgs = {
            .rangeCount = rangeCount,
            .ppSpvPushConstants = pushConstantRanges.data(),
            .shaderStage = args.shaderStage,
            .pDstRanges = reflectRanges.data()
        };

        VkBridgeResult result = vkBridgeGetShaderStagePushConstantRanges(getRangesArgs);
        if (result != VK_BRIDGE_SUCCESS) {
            return result;
        }

        uint32_t setCount = 0;
        reflectResult = spvReflectEnumerateDescriptorSets(args.pSpvModule, &setCount, NULL);
        if (reflectResult != SPV_REFLECT_RESULT_SUCCESS) {
            return VK_BRIDGE_FAILURE;
        }

        std::vector<SpvReflectDescriptorSet*> spvSets(setCount);
        reflectResult = spvReflectEnumerateDescriptorSets(args.pSpvModule, &setCount, spvSets.data());
        if (reflectResult != SPV_REFLECT_RESULT_SUCCESS) {
            return VK_BRIDGE_FAILURE;
        }

        reflection.descriptorSetCount = setCount;
        result = vkBridgeGetShaderStageDescriptorSets((VkBridgeGetShaderStageDescriptorSetsArgs) {
            .setCount = setCount,
            .ppSpvSets = spvSets.data(),
            .shaderStageFlags = args.shaderStage,
            .ppStageDescriptorSets = &reflection.pDescriptorSets
        });
        if (result != VK_BRIDGE_SUCCESS) {
            return result;
        }

        if (args.shaderStage & VK_SHADER_STAGE_VERTEX_BIT) {
            uint32_t inputCount = 0;
            VkBridgeGetShaderStageInputsArgs getInputsArgs = {
                .pShaderModule = args.pSpvModule,
                .excludeDecorationFlags = args.shaderInputsExcludeDecorationFlags,
                .pInputCount = &inputCount,
                .pInputs = NULL
            };

            reflectResult = vkBridgeGetShaderStageInputs(getInputsArgs);
            if (reflectResult != SPV_REFLECT_RESULT_SUCCESS) {
                vkBridgeDestroyShaderStageReflection(&reflection);
                return VK_BRIDGE_FAILURE;
            }

            inputs.resize(inputCount);
            getInputsArgs.pInputs = inputs.data();
            reflectResult = vkBridgeGetShaderStageInputs(getInputsArgs);
            if (reflectResult != SPV_REFLECT_RESULT_SUCCESS) {
                vkBridgeDestroyShaderStageReflection(&reflection);
                return VK_BRIDGE_FAILURE;
            }

            for (uint32_t i = 0; i < inputs.size(); i++) {
                VkBridgeShaderStageInput* pInput = &inputs[i];
                const VkBridgeShaderStageInputAdditionalInfo* pAdditionalInputInfo = NULL;

            for (uint32_t additionalInfoIdx = 0; additionalInfoIdx < args.shaderInputAdditionalInfoCount; additionalInfoIdx++) {
                    const VkBridgeNamedShaderStageInputInfo* pAdditionalInput = &args.pShaderInputAdditionalInfos[additionalInfoIdx];
                    if (pAdditionalInput->name != NULL && strcmp(pAdditionalInput->name, pInput->name) == 0) {
                        pAdditionalInputInfo = &pAdditionalInput->info;
                    break;
                }
            }

                if (pAdditionalInputInfo == NULL) {
                    vkBridgeDestroyShaderStageInputs(inputCount, inputs.data());
                    vkBridgeDestroyShaderStageReflection(&reflection);
                    return VK_BRIDGE_ERROR_NOT_FOUND;
            }

                VkVertexInputAttributeDescription description = {
                    .location = pInput->location,
                    .binding = pAdditionalInputInfo->binding,
                    .format = pInput->format,
                    .offset = pAdditionalInputInfo->offset
            };

            reflectInputAttributeDescriptions.push_back(description);
        }

            vkBridgeDestroyShaderStageInputs(inputCount, inputs.data());
        }

        uint32_t mapEntryCount = 0;

        VkBridgeGetSpecializationMapEntriesArgs getSpecializationMapEntriesArgs = {
            .pShaderModule = args.pSpvModule,
            .srcConstantCount = args.specializationConstantSourceCount,
            .pSources = args.pSpecializationConstantSources,
            .pEntryCount = &mapEntryCount, // same as srcConstantCount
            .pEntries = NULL
        };

        reflectResult = vkBridgeGetSpecializationMapEntries(getSpecializationMapEntriesArgs);
        if (reflectResult != SPV_REFLECT_RESULT_SUCCESS) {
            vkBridgeDestroyShaderStageReflection(&reflection);
            return VK_BRIDGE_FAILURE;
        }

        specializationMapEntries.resize(mapEntryCount);
        getSpecializationMapEntriesArgs.pEntries = specializationMapEntries.data();

        reflectResult = vkBridgeGetSpecializationMapEntries(getSpecializationMapEntriesArgs);
        if (reflectResult != SPV_REFLECT_RESULT_SUCCESS) {
            vkBridgeDestroyShaderStageReflection(&reflection);
            return VK_BRIDGE_FAILURE;
        }

        size_t specializationDataSize = 0;
        for (uint32_t sourceIdx = 0; sourceIdx < args.specializationConstantSourceCount; sourceIdx++) {
            const VkBridgeSpecializationConstantSource* pSource = &args.pSpecializationConstantSources[sourceIdx];
            const size_t sourceEnd = (size_t)pSource->offset + pSource->size;
            if (sourceEnd > specializationDataSize) {
                specializationDataSize = sourceEnd;
            }
        }

        if (specializationDataSize > 0 && args.pSpecializationData == NULL) {
            vkBridgeDestroyShaderStageReflection(&reflection);
            return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
        }

        reflection.pushConstantRangeCount = (uint32_t)reflectRanges.size();
        if (reflection.pushConstantRangeCount > 0) {
            reflection.pPushConstantRanges = (VkBridgePushConstantRange*)calloc(
                reflection.pushConstantRangeCount,
                sizeof(VkBridgePushConstantRange)
            );
            if (reflection.pPushConstantRanges == NULL) {
                vkBridgeDestroyShaderStageReflection(&reflection);
                return VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY;
            }
            memcpy(reflection.pPushConstantRanges, reflectRanges.data(), reflectRanges.size() * sizeof(reflectRanges[0]));
        }

        reflection.inputAttributeDescriptionCount = (uint32_t)reflectInputAttributeDescriptions.size();
        if (reflection.inputAttributeDescriptionCount > 0) {
            reflection.pInputAttributeDescriptions = (VkVertexInputAttributeDescription*)calloc(
                reflection.inputAttributeDescriptionCount,
                sizeof(VkVertexInputAttributeDescription)
            );
            if (reflection.pInputAttributeDescriptions == NULL) {
                vkBridgeDestroyShaderStageReflection(&reflection);
                return VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY;
            }
            memcpy(
                reflection.pInputAttributeDescriptions,
                reflectInputAttributeDescriptions.data(),
                reflectInputAttributeDescriptions.size() * sizeof(reflectInputAttributeDescriptions[0])
            );
        }

        reflection.specializationMapEntryCount = (uint32_t)specializationMapEntries.size();
        if (reflection.specializationMapEntryCount > 0) {
            reflection.pSpecializationMapEntries = (VkBridgeSpecializationMapEntry*)calloc(
                reflection.specializationMapEntryCount,
                sizeof(VkBridgeSpecializationMapEntry)
            );
            if (reflection.pSpecializationMapEntries == NULL) {
                vkBridgeDestroyShaderStageReflection(&reflection);
                return VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY;
            }
            memcpy(
                reflection.pSpecializationMapEntries,
                specializationMapEntries.data(),
                specializationMapEntries.size() * sizeof(specializationMapEntries[0])
            );
        }

        reflection.specializationDataSize = specializationDataSize;
        (*args.pReflection) = reflection;
        return VK_BRIDGE_SUCCESS;
    }
    catch (const std::bad_alloc&) {
        vkBridgeDestroyShaderStageInputs((uint32_t)inputs.size(), inputs.data());
        vkBridgeDestroyShaderStageReflection(&reflection);
        return VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY;
    }
    catch (const std::length_error&) {
        vkBridgeDestroyShaderStageInputs((uint32_t)inputs.size(), inputs.data());
        vkBridgeDestroyShaderStageReflection(&reflection);
        return VK_BRIDGE_ERROR_LIMIT_EXCEEDED;
    }
}

void vkBridgeDestroyShaderStageReflection(VkBridgeShaderStageReflection* pReflection) {
    if (pReflection == NULL) {
        return;
    }

    free(pReflection->pPushConstantRanges);
    vkBridgeDestroyShaderStageDescriptorSets(pReflection->descriptorSetCount, pReflection->pDescriptorSets);
    free(pReflection->pInputAttributeDescriptions);
    free(pReflection->pSpecializationMapEntries);
    (*pReflection) = {};
}

VkBridgeResult vkBridgeMergeShaderStageReflections(VkBridgeMergeShaderStageReflectionsArgs args) {
    if (args.pPipelineReflection == NULL ||
        (args.shaderStageCount > 0 && args.ppShaderStageReflections == NULL)) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    (*args.pPipelineReflection) = {};
    VkBridgePipelineReflection pipelineReflection = {};

    try {
        std::vector<VkBridgePushConstantRange> allRanges;
        std::vector<uint32_t> stageSetCounts;
        std::vector<const VkBridgeShaderStageDescriptorSet*> stageDescriptorSets;

        stageSetCounts.reserve(args.shaderStageCount);
        stageDescriptorSets.reserve(args.shaderStageCount);

        size_t rangeCount = 0;
        for (uint32_t stageIdx = 0; stageIdx < args.shaderStageCount; stageIdx++) {
            const VkBridgeShaderStageReflection* stageReflection = args.ppShaderStageReflections[stageIdx];
            if (stageReflection == NULL) {
                return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
            }

            if (stageReflection->pushConstantRangeCount > SIZE_MAX - rangeCount) {
                return VK_BRIDGE_ERROR_LIMIT_EXCEEDED;
            }

            if ((stageReflection->descriptorSetCount > 0 && stageReflection->pDescriptorSets == NULL) ||
                (stageReflection->pushConstantRangeCount > 0 && stageReflection->pPushConstantRanges == NULL)) {
                return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
            }

            rangeCount += stageReflection->pushConstantRangeCount;
            stageSetCounts.push_back(stageReflection->descriptorSetCount);
            stageDescriptorSets.push_back(stageReflection->pDescriptorSets);
        }

        if (rangeCount > UINT32_MAX / 2) {
            return VK_BRIDGE_ERROR_LIMIT_EXCEEDED;
        }

        allRanges.reserve(rangeCount);
        for (uint32_t stageIdx = 0; stageIdx < args.shaderStageCount; stageIdx++) {
            const VkBridgeShaderStageReflection* stageReflection = args.ppShaderStageReflections[stageIdx];
            if (stageReflection->pushConstantRangeCount > 0) {
                allRanges.insert(
                    allRanges.end(),
                    stageReflection->pPushConstantRanges,
                    stageReflection->pPushConstantRanges + stageReflection->pushConstantRangeCount
                );
            }
        }

        uint32_t mergedRangeCount = 0;
        VkBridgeMergePushConstantRangesArgs mergeRangesArgs = {
            .rangeCount = (uint32_t)allRanges.size(),
            .pRanges = allRanges.data(),
            .pMergedRangeCount = &mergedRangeCount,
            .pMergedRanges = NULL
        };

        VkBridgeResult result = vkBridgeMergePushConstantRanges(mergeRangesArgs);
        if (result != VK_BRIDGE_SUCCESS) {
            return result;
        }

        pipelineReflection.pushConstantRangeCount = mergedRangeCount;
        if (mergedRangeCount > 0) {
            pipelineReflection.pPushConstantRanges = (VkBridgePushConstantRange*)calloc(
                mergedRangeCount,
                sizeof(VkBridgePushConstantRange)
            );
            if (pipelineReflection.pPushConstantRanges == NULL) {
                return VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY;
            }
        }
        mergeRangesArgs.pMergedRanges = pipelineReflection.pPushConstantRanges;

        result = vkBridgeMergePushConstantRanges(mergeRangesArgs);
        if (result != VK_BRIDGE_SUCCESS) {
            vkBridgeDestroyPipelineReflection(&pipelineReflection);
            return result;
        }

        uint32_t descriptorSetCount = 0;
        VkBridgeMergeShaderStageDescriptorSetsArgs mergeDescriptorSetsArgs = {
            .shaderStageCount = args.shaderStageCount,
            .pStageSetCounts = stageSetCounts.data(),
            .ppStageDescriptorSets = stageDescriptorSets.data(),
            .pDescriptorSetCount = &descriptorSetCount,
            .ppDstDescriptorSets = NULL
        };

        result = vkBridgeMergeShaderStageDescriptorSets(mergeDescriptorSetsArgs);
        if (result != VK_BRIDGE_SUCCESS) {
            vkBridgeDestroyPipelineReflection(&pipelineReflection);
            return result;
        }

        pipelineReflection.descriptorSetCount = descriptorSetCount;
        mergeDescriptorSetsArgs.ppDstDescriptorSets = &pipelineReflection.pDescriptorSets;

        result = vkBridgeMergeShaderStageDescriptorSets(mergeDescriptorSetsArgs);
        if (result != VK_BRIDGE_SUCCESS) {
            vkBridgeDestroyPipelineReflection(&pipelineReflection);
            return result;
        }

        (*args.pPipelineReflection) = pipelineReflection;

        return VK_BRIDGE_SUCCESS;
    }
    catch (const std::bad_alloc&) {
        vkBridgeDestroyPipelineReflection(&pipelineReflection);
        return VK_BRIDGE_ERROR_OUT_OF_HOST_MEMORY;
    }
    catch (const std::length_error&) {
        vkBridgeDestroyPipelineReflection(&pipelineReflection);
        return VK_BRIDGE_ERROR_LIMIT_EXCEEDED;
    }
}

void vkBridgeDestroyPipelineReflection(VkBridgePipelineReflection* pReflection) {
    if (pReflection == NULL) {
        return;
    }

    free(pReflection->pPushConstantRanges);
    vkBridgeDestroyDescriptorSets(pReflection->descriptorSetCount, pReflection->pDescriptorSets);
    (*pReflection) = {};
}

VkBridgeResult vkBridgeGetDescriptorSetLayoutCreateInfo(VkBridgeGetDescriptorSetLayoutCreateInfoArgs args) {

    if (args.pDescriptorSet == NULL || args.pCreateInfo == NULL) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    if (args.pDescriptorSet->setLayoutBindingCount > 0 && args.pDescriptorSet->pSetLayoutBindings == NULL) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    (*args.pCreateInfo) = (VkDescriptorSetLayoutCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = args.pDescriptorSet->setLayoutBindingCount,
        .pBindings = args.pDescriptorSet->pSetLayoutBindings
    };

    return VK_BRIDGE_SUCCESS;
}

VkBridgeResult vkBridgeGetPipelineLayoutCreateInfo(VkBridgeGetPipelineLayoutCreateInfoArgs args) {

    if (args.pReflection == NULL || args.pCreateInfo == NULL ||
        (args.setLayoutCount > 0 && args.pSetLayouts == NULL)) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    if (args.pReflection->descriptorSetCount != args.setLayoutCount) {
        return VK_BRIDGE_ERROR_INCOMPATIBLE_DATA;
    }

    if (args.pReflection->pushConstantRangeCount > 0 && args.pReflection->pPushConstantRanges == NULL) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    (*args.pCreateInfo) = (VkPipelineLayoutCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = args.setLayoutCount,
        .pSetLayouts = args.pSetLayouts,
        .pushConstantRangeCount = args.pReflection->pushConstantRangeCount,
        .pPushConstantRanges = args.pReflection->pPushConstantRanges
    };

    return VK_BRIDGE_SUCCESS;
}

VkBridgeResult vkBridgeGetSpecializationInfo(VkBridgeGetSpecializationInfoArgs args) {

    if (args.pReflection == NULL || args.pSpecializationInfo == NULL) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    if (args.pReflection->specializationMapEntryCount > 0 && args.pReflection->pSpecializationMapEntries == NULL) {
        return VK_BRIDGE_ERROR_INVALID_ARGUMENT;
    }

    if (args.pReflection->specializationDataSize > 0 && args.pReflection->pSpecializationData == NULL) {
        return VK_BRIDGE_ERROR_INCOMPATIBLE_DATA;
    }

    (*args.pSpecializationInfo) = (VkSpecializationInfo) {
        .mapEntryCount = args.pReflection->specializationMapEntryCount,
        .pMapEntries = args.pReflection->pSpecializationMapEntries,
        .dataSize = args.pReflection->specializationDataSize,
        .pData = args.pReflection->pSpecializationData
    };

    return VK_BRIDGE_SUCCESS;
}
