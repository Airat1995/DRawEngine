#pragma once
#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>
#include "BaseBuffer.h"

using namespace std;

class VulkanBuffer
{
public:
	VulkanBuffer(VkDevice device, VkPhysicalDevice physical, BufferStageFlag stageFlag, BufferUsageFlag usage, BufferSharingMode sharingMode, const void* data, int size, int bindingId);
	
	virtual void Fill();

	virtual void Flush();

	virtual ~VulkanBuffer();

	virtual void Destroy();

	VkBuffer& Buffer();

	int BindingId() const;

	VkDescriptorSetLayout DescriptorSetLayout() const;

	VkDescriptorBufferInfo BufferDescriptorInfo();

protected:
	VkBuffer _buffer;
	VkDeviceMemory _memory;
	VkDescriptorBufferInfo _bufferDescriptorInfo;
	VkDescriptorSetLayoutBinding _layoutBinding;
	VkDevice _device;
	VkBufferCreateInfo _bufferInfo;
	VkMemoryRequirements _memoryReq;
	VkPhysicalDevice _physical;
	BufferUsageFlag _usage;
	BufferSharingMode _sharingMode;
	VkDescriptorSetLayout _descriptorSetLayout;

	int _size;
	int _bindingId;
	int _dataSize;
	const void* _dataLocation;
	uint8_t* _dataPointer = nullptr;

	static uint32_t FindMemoryType(uint32_t typeFilter, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryPropertyFlags properties);

	static VkShaderStageFlagBits GetUsage(BufferStageFlag stage);
};