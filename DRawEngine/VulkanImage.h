#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>

#include "IImage.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"

using namespace std;

class VulkanImage : public IImage
{
public:
	explicit VulkanImage(VulkanCommandPool* commandPool, ImageFormat format, ImageType type, ImageUsage imageUsage, BufferStageFlag stage,
	                     int width, int height, vector<unsigned char>& imageData, VkDevice device,
	                     VkPhysicalDevice gpu, int binding, int graphicsFamilyIndex, int samples);
	void Clean() const;

	VkDescriptorSetLayout DescriptorSetLayout() const;

	VkDescriptorImageInfo ImageInfo() const;

	VkDescriptorSetLayoutBinding DescriptorBindingInfo() const;

	~VulkanImage();
protected:

	bool MemoryTypeFromProperties(uint32_t typeBits, VkFlags requirements_mask, uint32_t* typeIndex);

	void CreateCopyCommandBuffer(VkImageUsageFlagBits imageUsage);

	void CreateSampler();

	void CreateDescriptorSetLayout();

	vector<VkBufferImageCopy> CreateRegion() const;

	static VkImageMemoryBarrier CreateImageMemoryBarrier();

	VkImage _image;

	VkDeviceMemory _mem;

	VkImageView _view;

	VkImageLayout _imageLayout;

	VkDescriptorSetLayout _descriptorSetLayout;
	
	VkPhysicalDeviceMemoryProperties _memoryProperties;
	
	VkDevice _device;

	VulkanBuffer* _buffer;

	VulkanCommandBuffer* _commandBuffer;

	VulkanCommandPool* _commandPool;

	VkQueue _imageSubmitQueue;

	VkSampler _sampler;

	VkDescriptorImageInfo _imageInfo;

	VkDescriptorSetLayoutBinding _samplerLayoutBinding;

	int _graphicsFamilyIndex;

	static VkFormat ImageFormatToVulkan(ImageFormat format);

	static VkImageType ImageTypeToVulkan(ImageType type);

	static VkImageViewType ImageTypeToVulkanViewType(ImageType type);

	static VkImageUsageFlagBits ImageUsageToVulkan(ImageUsage usage);

	static int ImageCount(vector<unsigned char>& image, ImageType imageType, int width, int height, int channelsCount);

	static int ChannelsCount(ImageFormat format);
};

