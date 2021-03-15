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
	
	static uint32_t FindMemoryType(uint32_t typeFilter, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryPropertyFlags properties);

	void Fill();

	void Flush();

	virtual ~VulkanBuffer();

	void Destroy();

	VkBuffer& Buffer();

	int Binding() const;

	int Size() const;

	const void* DataLocation();

	VkDescriptorSetLayout DescriptorSetLayout() const;

	VkDescriptorSetLayoutBinding DescriptorBindingInfo() const;
	
	VkDescriptorBufferInfo* BufferDescriptorInfo();

	static VkDescriptorType BufferUsageToDescriptorType(BufferUsageFlag bufferUsageFlag);
	
	static VkShaderStageFlagBits GetUsage(BufferStageFlag stage);
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


};