#include "IImage.h"

IImage::IImage(VkFormat format, VkDevice* device, vector<VkPhysicalDevice>* gpus, VkImageViewType imageType, int width, int height, VkSampleCountFlagBits samples)
{
	_device = device;

	VkImageCreateInfo image_info = {};
	VkFormatProperties props;
	vkGetPhysicalDeviceFormatProperties(gpus->at(0), format, &props);
	if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) 
	{
		image_info.tiling = VK_IMAGE_TILING_LINEAR;
	}
	else if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) 
	{
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	}
	else 
	{
		/* Try other depth formats? */
		std::cout << "VK_FORMAT_D16_UNORM Unsupported.\n";
		exit(-1);
	}

	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.pNext = nullptr;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.format = format;
	image_info.extent.width = width;
	image_info.extent.height = height;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;
	image_info.samples = samples;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	image_info.queueFamilyIndexCount = 0;
	image_info.pQueueFamilyIndices = nullptr;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_info.flags = 0;

	VkMemoryAllocateInfo mem_alloc = {};
	mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mem_alloc.pNext = nullptr;
	mem_alloc.allocationSize = 0;
	mem_alloc.memoryTypeIndex = 0;

	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.pNext = nullptr;
	view_info.image = nullptr;
	view_info.format = format;
	view_info.components.r = VK_COMPONENT_SWIZZLE_R;
	view_info.components.g = VK_COMPONENT_SWIZZLE_G;
	view_info.components.b = VK_COMPONENT_SWIZZLE_B;
	view_info.components.a = VK_COMPONENT_SWIZZLE_A;
	view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;
	view_info.viewType = imageType;
	view_info.flags = 0;

	VkMemoryRequirements memReqs;

	_format = format;

	/* Create image */
	VkResult res = vkCreateImage(*device, &image_info, nullptr, &_image);
	if(res != VK_SUCCESS)
	{
		std::cerr << "Unable to create image!" << std::endl;
		return;
	}
	

	vkGetImageMemoryRequirements(*device, _image, &memReqs);

	mem_alloc.allocationSize = memReqs.size;
	/* Use the memory properties to determine the type of memory required */
	bool pass = MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mem_alloc.memoryTypeIndex);
	if(!pass)
	{
		std::cerr << "Unable to get memory properties from image!" << std::endl;
	}

	/* Allocate memory */
	res = vkAllocateMemory(*device, &mem_alloc, nullptr, &_mem);
	if (!pass)
	{
		std::cerr << "Unable to get memory properties from image!" << std::endl;
	}

	/* Bind memory */
	res = vkBindImageMemory(*device, _image, _mem, 0);
	if (res != VK_SUCCESS)
	{
		std::cerr << "Unable to bind image memory!" << std::endl;
	}

	view_info.image = _image;
	res = vkCreateImageView(*device, &view_info, nullptr, &_view);
	if(res != VK_SUCCESS)
	{
		std::cerr << "Unable to create view for the image!" << std::endl;
	}

}

IImage::~IImage()
{
	vkDestroyImageView(*_device, _view, nullptr);
	vkDestroyImage(*_device, _image, nullptr);
	vkFreeMemory(*_device, _mem, nullptr);
}

bool IImage::MemoryTypeFromProperties(uint32_t typeBits, VkFlags requirements_mask, uint32_t* typeIndex) 
{
	// Search memtypes to find first index with those properties
	for (uint32_t i = 0; i < _memoryProperties.memoryTypeCount; i++) 
	{
		if ((typeBits & 1) == 1) 
		{
			// Type is available, does it match user properties?
			if ((_memoryProperties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) 
			{
				*typeIndex = i;
				return true;
			}
		}
		typeBits >>= 1;
	}
	// No memory types matched, return failure
	return false;
}

