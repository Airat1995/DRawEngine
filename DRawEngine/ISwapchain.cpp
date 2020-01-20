#include "ISwapchain.h"

ISwapchain::ISwapchain(VkDevice* device, VkExtent2D swapchainExtent, VkSurfaceCapabilitiesKHR surfaceCapabilities,
                       VkSurfaceKHR surface, vector<VkPhysicalDevice>* gpus, uint32_t graphicsQueueFamilyIndex, uint32_t presentQueueFamilyIndex) : _device(device)
{
	VkFormat supportedFormat = GetSupportedFormat(gpus, surface);

	const VkSurfaceTransformFlagBitsKHR preTransform = GetPresentMode(surfaceCapabilities);
	const VkCompositeAlphaFlagBitsKHR compositeAlpha = GetCompositeAlpha(surfaceCapabilities);
	const VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	uint32_t desiredNumberOfSwapChainImages = surfaceCapabilities.minImageCount;

	if (surfaceCapabilities.maxImageCount > 0 && desiredNumberOfSwapChainImages > surfaceCapabilities.maxImageCount)
		desiredNumberOfSwapChainImages = surfaceCapabilities.maxImageCount;

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	
	_swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	_swapchainInfo.pNext = nullptr;
	_swapchainInfo.surface = surface;
	_swapchainInfo.minImageCount = desiredNumberOfSwapChainImages;
	_swapchainInfo.imageFormat = supportedFormat;
	_swapchainInfo.imageExtent.width = swapchainExtent.width;
	_swapchainInfo.imageExtent.height = swapchainExtent.height;
	_swapchainInfo.preTransform = preTransform;
	_swapchainInfo.compositeAlpha = compositeAlpha;
	_swapchainInfo.imageArrayLayers = 1;
	_swapchainInfo.presentMode = swapchainPresentMode;
	_swapchainInfo.oldSwapchain = nullptr;
	_swapchainInfo.clipped = true;
	_swapchainInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	_swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	_swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	_swapchainInfo.queueFamilyIndexCount = 0;
	_swapchainInfo.pQueueFamilyIndices = nullptr;

	uint32_t queueFamilyIndices[2] = { graphicsQueueFamilyIndex, presentQueueFamilyIndex };
	if (presentQueueFamilyIndex != graphicsQueueFamilyIndex)
	{
		_swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		_swapchainInfo.queueFamilyIndexCount = 2;
		_swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		_swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		_swapchainInfo.queueFamilyIndexCount = 0; // Optional
		_swapchainInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	CreateSwapchainAndImages(device, _swapchainInfo);
	CreateSemaphore();

}

VkSwapchainCreateInfoKHR* ISwapchain::SwapchainInfo()
{
	return &_swapchainInfo;
}

vector<SwapchainBuffer>* ISwapchain::SwapchainBuffers()
{
	return _swapchainBuffers;
}

ISwapchain::~ISwapchain()
{
	vkDestroySwapchainKHR(*_device, _swapChain, nullptr);
}


void ISwapchain::CreateSwapchainAndImages(VkDevice* device, VkSwapchainCreateInfoKHR swapchainInfo)
{
	VkResult result = vkCreateSwapchainKHR(*device, &swapchainInfo, nullptr, &_swapChain);
	if (result != VK_SUCCESS)
	{
		throw std::exception("Unable to create swapchain!");
	}

	uint32_t imageCount;
	result = vkGetSwapchainImagesKHR(*device, _swapChain, &imageCount, nullptr);
	if (result != VK_SUCCESS)
	{
		throw std::exception("Unable to create swapchain!");
	}
	VkImage* swapchainImages = new VkImage[imageCount];
	result = vkGetSwapchainImagesKHR(*device, _swapChain, &imageCount, swapchainImages);
	if (result != VK_SUCCESS)
	{
		throw std::exception("Unable to create swapchain!");
	}

	_swapchainBuffers = new std::vector<SwapchainBuffer>(imageCount);
	for (uint32_t index = 0; index < imageCount; ++index)
	{
		_swapchainBuffers->at(index).Image(swapchainImages[index]);
	}
	delete[](swapchainImages);

	for (uint32_t i = 0; i < imageCount; i++) {
		VkImageViewCreateInfo color_image_view = {};
		color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		color_image_view.pNext = nullptr;
		color_image_view.flags = 0;
		color_image_view.image = *_swapchainBuffers->at(i).Image();
		color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
		color_image_view.format = swapchainInfo.imageFormat;
		color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
		color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
		color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
		color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
		color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		color_image_view.subresourceRange.baseMipLevel = 0;
		color_image_view.subresourceRange.levelCount = 1;
		color_image_view.subresourceRange.baseArrayLayer = 0;
		color_image_view.subresourceRange.layerCount = 1;

		VkResult resultImage = vkCreateImageView(*device, &color_image_view, nullptr, _swapchainBuffers->at(i).View());
		if (resultImage != VK_SUCCESS)
		{
			throw std::exception("Unable to create image!");
		}
	}

}

VkSurfaceTransformFlagBitsKHR ISwapchain::GetPresentMode(VkSurfaceCapabilitiesKHR surfaceCapabilities)
{
	VkSurfaceTransformFlagBitsKHR preTransform;
	if ((surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) == 1)
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	else
		preTransform = surfaceCapabilities.currentTransform;

	return preTransform;
}

VkCompositeAlphaFlagBitsKHR ISwapchain::GetCompositeAlpha(VkSurfaceCapabilitiesKHR surfaceCapabilities)
{
	VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] =
	{
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
	};

	for (auto& compositeAlphaFlag : compositeAlphaFlags)
	{
		if ((surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag) == 0)
			continue;
		compositeAlpha = compositeAlphaFlag;
		break;
	}

	return  compositeAlpha;
}

VkFormat ISwapchain::GetSupportedFormat(vector<VkPhysicalDevice>* gpus, VkSurfaceKHR surface)
{
	uint32_t formatCount;
	VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
	VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(gpus->at(0), surface, &formatCount, nullptr);
	if (result != VK_SUCCESS)
		throw exception("unable to get device surface image formats count!");

	auto surfFormats = new VkSurfaceFormatKHR[formatCount];
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(gpus->at(0), surface, &formatCount, surfFormats);
	if (result != VK_SUCCESS)
		throw exception("unable to get device surface image formats!");

	if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED)
		format = VK_FORMAT_B8G8R8A8_UNORM;
	else
		format = surfFormats[0].format;

	return format;
}

void ISwapchain::CreateSemaphore()
{
	VkSemaphoreCreateInfo semaphoreCreateInfo =
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
  nullptr,
  0
	};

	VkResult result = vkCreateSemaphore(*_device, &semaphoreCreateInfo, nullptr, &_imageAvailableSemaphore);
	if (result != VK_SUCCESS)
		cerr << "Unable to create semaphore for the available image!" << endl;
	result = vkCreateSemaphore(*_device, &semaphoreCreateInfo, nullptr, &_renderFinishSemaphore);
	if (result != VK_SUCCESS)
		cerr << "Unable to create semaphore for the render finish!" << endl;
}
