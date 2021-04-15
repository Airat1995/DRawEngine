#include "VulkanImage.h"


VulkanImage::VulkanImage(VulkanCommandPool* commandPool, ImageFormat format, ImageType type, ImageUsage imageUsage, BufferStageFlag stage,
                         int width, int height, vector<unsigned char>& imageData, VkDevice device,
                         VkPhysicalDevice gpu, int binding, int graphicsFamilyIndex, int samples)
	: IImage(format, type, imageUsage, stage, binding, width, height, samples, imageData), _commandPool(commandPool), _graphicsFamilyIndex(graphicsFamilyIndex)
{
	_device = device;
	int channelsCount = BufferlessVulkanImage::ChannelsCount(format);
	int elementsCount = BufferlessVulkanImage::ImageCount(imageData, type, width, height, channelsCount);
	_buffer = new VulkanBuffer(_device, gpu, stage, BufferUsageFlag::TransferSrc, BufferSharingMode::Exclusive, imageData.data(),
		elementsCount * width * height * channelsCount, binding);
	_buffer->Fill();
	CreateDescriptorSetLayout();
	CreateSampler();

	VkFormat vulkanFormat = BufferlessVulkanImage::ImageFormatToVulkan(format);
	VkImageType vkImageType = BufferlessVulkanImage::ImageTypeToVulkan(type);
	VkImageViewType imageView = BufferlessVulkanImage::ImageTypeToVulkanViewType(type);
	VkImageUsageFlagBits vulkanImageUsage = BufferlessVulkanImage::ImageUsageToVulkan(imageUsage);
	
	VkImageCreateInfo image_info = {};
	VkFormatProperties props;
	vkGetPhysicalDeviceFormatProperties(gpu, vulkanFormat, &props);
	vkGetPhysicalDeviceMemoryProperties(gpu, &_memoryProperties);
	
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.pNext = nullptr;
	image_info.imageType = vkImageType;
	image_info.format = vulkanFormat;
	image_info.extent.width = width;
	image_info.extent.height = height;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = elementsCount;
	image_info.samples = static_cast<VkSampleCountFlagBits>(samples);
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = vulkanImageUsage;
	image_info.queueFamilyIndexCount = 0;
	image_info.pQueueFamilyIndices = nullptr;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_info.flags = 0;
	if(_type == ImageType::Cube)
	{
		image_info.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	}

	VkMemoryAllocateInfo mem_alloc = {};
	mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mem_alloc.pNext = nullptr;
	mem_alloc.allocationSize = 0;
	mem_alloc.memoryTypeIndex = 0;

	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.pNext = nullptr;
	view_info.image = nullptr;
	view_info.format = vulkanFormat;
	view_info.components.r = VK_COMPONENT_SWIZZLE_R;
	view_info.components.g = VK_COMPONENT_SWIZZLE_G;
	view_info.components.b = VK_COMPONENT_SWIZZLE_B;
	view_info.components.a = VK_COMPONENT_SWIZZLE_A;
	view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = elementsCount;
	view_info.viewType = imageView;
	view_info.flags = 0;

	VkMemoryRequirements memReqs;

	// Create image
	VkResult res = vkCreateImage(device, &image_info, nullptr, &_image);
	if(res != VK_SUCCESS)
	{
		std::cerr << "Unable to create image!" << std::endl;
		return;
	}	

	vkGetImageMemoryRequirements(device, _image, &memReqs);

	mem_alloc.allocationSize = memReqs.size;
	// Use the memory properties to determine the type of memory required
	bool pass = BufferlessVulkanImage::MemoryTypeFromProperties(_memoryProperties,
		memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mem_alloc.memoryTypeIndex);

	if(!pass)
	{
		std::cerr << "Unable to get memory properties from image!" << std::endl;
	}

	// Allocate memory
	res = vkAllocateMemory(device, &mem_alloc, nullptr, &_mem);
	if (!pass)
	{
		std::cerr << "Unable to get memory properties from image!" << std::endl;
	}

	// Bind memory
	res = vkBindImageMemory(device, _image, _mem, 0);
	if (res != VK_SUCCESS)
	{
		std::cerr << "Unable to bind image memory!" << std::endl;
	}

	view_info.image = _image;
	res = vkCreateImageView(device, &view_info, nullptr, &_view);
	if(res != VK_SUCCESS)
	{
		std::cerr << "Unable to create view for the image!" << std::endl;
	}
	CreateCopyCommandBuffer(vulkanImageUsage);

	_imageInfo = {};
	_imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	_imageInfo.imageView = _view;
	_imageInfo.sampler = _sampler;
}

void VulkanImage::Clean() const
{
	vkDestroyImageView(_device, _view, nullptr);
	vkDestroyImage(_device, _image, nullptr);
	vkFreeMemory(_device, _mem, nullptr);
}

VkImageView VulkanImage::View() const
{
	return _view;
}

VkDescriptorSetLayout VulkanImage::DescriptorSetLayout() const
{
	return _descriptorSetLayout;
}

VkDescriptorImageInfo VulkanImage::ImageInfo() const
{
	return _imageInfo;
}

VulkanImage::~VulkanImage()
{	
}

void VulkanImage::CreateCopyCommandBuffer(VkImageUsageFlagBits imageUsage)
{
	int layerCount = _type == ImageType::Cube ? 6 : 1;
	_commandBuffer = &_commandPool->CommandBuffer(0);
	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.layerCount = layerCount;
	VkImageMemoryBarrier imageBarrier = BufferlessVulkanImage::CreateImageMemoryBarrier();
	imageBarrier.image = _image;
	imageBarrier.subresourceRange = subresourceRange;
	imageBarrier.srcAccessMask = 0;
	imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

	_commandBuffer->BeginCommandBuffer();
	
	vkCmdPipelineBarrier(
		_commandBuffer->CommandBuffer(),
		VK_PIPELINE_STAGE_HOST_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageBarrier);

	// Once the data has been uploaded we transfer to the texture image to the shader read layout, so it can be sampled from
	imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	vector<VkBufferImageCopy> imageCopies = CreateRegion();
	
	vkCmdCopyBufferToImage(
		_commandBuffer->CommandBuffer(),
		_buffer->Buffer(),
		_image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		imageCopies.size(),
		imageCopies.data());

	vkCmdPipelineBarrier(
		_commandBuffer->CommandBuffer(),
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageBarrier);

	_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	vkGetDeviceQueue(_device, _graphicsFamilyIndex, 0, &_imageSubmitQueue);

	_commandBuffer->EndCommandBuffer();

	VkCommandBuffer commandBuffer = { _commandBuffer->CommandBuffer() };
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkResult result = vkQueueSubmit(_imageSubmitQueue, 1, &submitInfo, nullptr);
	if(result != VK_SUCCESS)
		throw exception("Unable to submit image's barrier queue");

	result = vkQueueWaitIdle(_imageSubmitQueue);
	if (result != VK_SUCCESS)
		throw exception("Unable to wait image's barrier queue");
}

void VulkanImage::CreateSampler()
{
	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.pNext = nullptr;
	samplerCreateInfo.maxAnisotropy = 1.0f;

	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;
	samplerCreateInfo.maxAnisotropy = 1.0;
	samplerCreateInfo.anisotropyEnable = VK_FALSE;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

	VkResult result = vkCreateSampler(_device, &samplerCreateInfo, nullptr, &_sampler);
	if (result != VK_SUCCESS)
		throw exception("Unable to create sampler!");
}

vector<VkBufferImageCopy> VulkanImage::CreateRegion() const
{
	int imageCount = _type == ImageType::Cube ? 6 : 1;
	int channelCount = BufferlessVulkanImage::ChannelsCount(_format);
	vector<VkBufferImageCopy> copyRegions = vector<VkBufferImageCopy>();
	for (int imageIndex = 0; imageIndex < imageCount; ++imageIndex)
	{
		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset = channelCount * (_height * _width) * imageIndex;
		copyRegion.bufferImageHeight = 0;
		copyRegion.bufferRowLength = 0;
		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.baseArrayLayer = imageIndex;
		copyRegion.imageSubresource.layerCount = 1;
		copyRegion.imageOffset = { 0,0,0 };
		copyRegion.imageExtent = { static_cast<uint32_t>(_width),static_cast<uint32_t>(_height), 1 };
		copyRegions.push_back(copyRegion);
	}

	return copyRegions;
}

void VulkanImage::CreateDescriptorSetLayout()
{
	_samplerLayoutBinding = {};
	_samplerLayoutBinding.binding = _binding;
	_samplerLayoutBinding.descriptorCount = 1;
	_samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	_samplerLayoutBinding.stageFlags = VulkanBuffer::GetUsage(_stage);
	_samplerLayoutBinding.pImmutableSamplers = nullptr;

}

void VulkanImage::CreateCopyCommandBuffer()
{
	vector<VkBufferImageCopy> imageCopies = CreateRegion();

	vkCmdCopyBufferToImage(
		_commandBuffer->CommandBuffer(),
		_buffer->Buffer(),
		_image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		imageCopies.size(),
		imageCopies.data());

	_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	vkGetDeviceQueue(_device, _graphicsFamilyIndex, 0, &_imageSubmitQueue);

	_commandBuffer->EndCommandBuffer();

	VkCommandBuffer commandBuffer = { _commandBuffer->CommandBuffer() };
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkResult result = vkQueueSubmit(_imageSubmitQueue, 1, &submitInfo, nullptr);
	if (result != VK_SUCCESS)
		throw exception("Unable to submit image's barrier queue");

	result = vkQueueWaitIdle(_imageSubmitQueue);
	if (result != VK_SUCCESS)
		throw exception("Unable to wait image's barrier queue");
}

VkDescriptorSetLayoutBinding VulkanImage::DescriptorBindingInfo() const
{
	return _samplerLayoutBinding;
}
