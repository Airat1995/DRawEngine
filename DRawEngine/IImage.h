#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>

using namespace std;

class IImage
{
public:
	explicit IImage(VkFormat format, VkDevice* device, vector<VkPhysicalDevice>* gpus, VkImageViewType imageType, int width, int height, VkSampleCountFlagBits samples);

	~IImage();
protected:

	bool MemoryTypeFromProperties(uint32_t typeBits, VkFlags requirements_mask, uint32_t* typeIndex);

	VkFormat _format;

	VkImage _image;

	VkDeviceMemory _mem;

	VkImageView _view;
	
	VkPhysicalDeviceMemoryProperties _memoryProperties;

	VkDevice* _device;
};

