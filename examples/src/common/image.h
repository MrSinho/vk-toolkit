#ifndef VK_TOOLKIT_IMAGE_H
#define VK_TOOLKIT_IMAGE_H

#include <vulkan/vulkan.h>



typedef struct VkToolkitImageData {
	VkFormat format;
	VkImage image;
	VkImageView view;
	VkMemoryRequirements memoryRequirements;
	VkDeviceSize memoryOffset;
} VkToolkitImageData;

static VkResult createImage2D(
	VkDevice device,
	VkExtent3D extent,
	VkFormat format,
	VkSampleCountFlagBits samples,
	VkImageUsageFlags usage,
	VkToolkitImageData* pImage
) {
	if (pImage == NULL) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkImageCreateInfo createInfo = {
		.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext                 = NULL,
		.flags                 = 0,
		.imageType             = VK_IMAGE_TYPE_2D,
		.format                = format,
		.extent                = extent,
		.mipLevels             = 1,
		.arrayLayers           = 1,
		.samples               = samples,
		.tiling                = VK_IMAGE_TILING_OPTIMAL,
		.usage                 = usage,
		.sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices   = NULL,
		.initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED
	};

	pImage->format = format;
	VkResult result = vkCreateImage(device, &createInfo, NULL, &pImage->image);
	if (result != VK_SUCCESS) {
		return result;
	}

	vkGetImageMemoryRequirements(device, pImage->image, &pImage->memoryRequirements);

	return VK_SUCCESS;
}

#endif//VK_TOOLKIT_IMAGE_H
