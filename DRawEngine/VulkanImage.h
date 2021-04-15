#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>

#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "BufferlessVulkanImage.h"

using namespace std;

class VulkanImage : public IImage
{
public:
	explicit VulkanImage(VulkanCommandPool* commandPool, ImageFormat format, ImageType type, ImageUsage imageUsage, BufferStageFlag stage,
	                     int width, int height, vector<unsigned char>& imageData, VkDevice device,
	                     VkPhysicalDevice gpu, int binding, int graphicsFamilyIndex, int samples);
	void Clean() const;

	VkImageView View() const;
	
	VkDescriptorSetLayout DescriptorSetLayout() const;

	VkDescriptorImageInfo ImageInfo() const;

	VkDescriptorSetLayoutBinding DescriptorBindingInfo() const;

	~VulkanImage();

protected:

	void CreateCopyCommandBuffer(VkImageUsageFlagBits imageUsage);

	void CreateSampler();

	vector<VkBufferImageCopy> CreateRegion() const;

	void CreateDescriptorSetLayout();
	
	void CreateCopyCommandBuffer();

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
};

