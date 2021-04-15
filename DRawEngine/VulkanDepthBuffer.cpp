#include "VulkanDepthBuffer.h"

VulkanDepthBuffer::VulkanDepthBuffer(VkDevice device, VkPhysicalDevice gpu, int widht, int height, bool useAsSampler)
: _device(device), _gpu(gpu), _width(widht), _height(height), _useAsSampler(useAsSampler)
{
	vector<VkFormat> depthFormats ={ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
	_depthFormat = FindSupportedFormat(depthFormats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

	CreateImage(_depthFormat);
	BindImage();
	CreateView(_depthFormat);
	_depthAttachment.format = _depthFormat;
	_depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	_depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	_depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	_depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	_depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	_depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	_depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

void VulkanDepthBuffer::Destroy()
{
	vkDestroyImage(_device, _image, nullptr);
	vkDestroyImageView(_device, _view, nullptr);
}

VkFormat VulkanDepthBuffer::Format()
{
	return _depthFormat;
}

VkImageView VulkanDepthBuffer::View()
{
	return _view;
}

VkFormat VulkanDepthBuffer::FindSupportedFormat(vector<VkFormat>& candidates, VkImageTiling tiling,
                                                VkFormatFeatureFlags features)
{
	for (auto format : candidates)
	{
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(_gpu, format, &properties);

		if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) 
		{
			return format;
		}

		if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) 
		{
			return format;
		}
	}

	throw runtime_error("Unable to find supported format to create depth buffer!");
}

bool VulkanDepthBuffer::HasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void VulkanDepthBuffer::CreateImage(VkFormat format)
{
	VkImageUsageFlags imageUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	if (_useAsSampler)
		imageUsage |= VK_IMAGE_USAGE_SAMPLED_BIT;
	
	VkImageCreateInfo image_info = {};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.pNext = nullptr;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.format = format;
	image_info.extent.width = _width;
	image_info.extent.height = _height;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	image_info.queueFamilyIndexCount = 0;
	image_info.pQueueFamilyIndices = nullptr;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_info.flags = 0;

	VkResult res = vkCreateImage(_device, &image_info, nullptr, &_image);
	if (res != VK_SUCCESS)
	{
		std::cerr << "Unable to create image for depth buffer!" << std::endl;
		return;
	}
}

void VulkanDepthBuffer::BindImage()
{
	VkMemoryRequirements memreq;
	vkGetImageMemoryRequirements(_device, _image, &memreq);
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(_gpu, &memProperties);

	VkMemoryAllocateInfo memAlloc = {};
	memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAlloc.pNext = nullptr;
	memAlloc.allocationSize = 0;
	memAlloc.memoryTypeIndex = 0;
	memAlloc.allocationSize = memreq.size;
	uint32_t pass = VulkanBuffer::FindMemoryType(memreq.memoryTypeBits, memProperties, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VkResult result = vkAllocateMemory(_device, &memAlloc, nullptr, &_memory);
	if (result != VK_SUCCESS)
		throw exception("Unable to allocate memory for depth memory!");
	result = vkBindImageMemory(_device, _image, _memory, 0);
	if (result != VK_SUCCESS)
		throw exception("Unable to bind memory for depth memory!");

}

void VulkanDepthBuffer::CreateView(VkFormat format)
{
	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.pNext = nullptr;
	view_info.image = nullptr;
	view_info.format = format;
	view_info.components.r = VK_COMPONENT_SWIZZLE_R;
	view_info.components.g = VK_COMPONENT_SWIZZLE_G;
	view_info.components.b = VK_COMPONENT_SWIZZLE_B;
	view_info.components.a = VK_COMPONENT_SWIZZLE_A;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;
	view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.flags = 0;
	view_info.image = _image;
	VkResult res = vkCreateImageView(_device, &view_info, nullptr, &_view);
	if (res != VK_SUCCESS)
	{
		std::cerr << "Unable to create view for depth buffer image!" << std::endl;
	}
}
