#pragma once
#include <vulkan/vulkan.h>

class SwapchainBuffer
{	

protected:
	VkImage _image = nullptr;

	VkImageView _view = nullptr;

public:

	VkImage* Image()
	{
		return &_image;
	}

	void Image(VkImage image)
	{
		_image = image;
	}

	VkImageView* View()
	{
		return &_view;
	}

	void View(VkImageView view)
	{
		_view = view;
	}
};
