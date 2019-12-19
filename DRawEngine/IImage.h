#pragma once
#include <vulkan/vulkan.h>
#include <vector>

using namespace std;

class IImage
{
public:
	explicit IImage(VkFormat format, vector<VkPhysicalDevice>* gpus, int width, int height);

	~IImage();
protected:
	VkFormat _format;

	VkImage _image;

	VkDeviceMemory _mem;

	VkImageView _view;
};

