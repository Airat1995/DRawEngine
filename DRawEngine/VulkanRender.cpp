#include "VulkanRender.h"
#include <cassert>


VulkanRender::VulkanRender() : IRender()
{
}

void VulkanRender::Init(vector<const char*>* extensions)
{	
	const VkApplicationInfo appInfo = CreateAppInfo();
	std::vector<const char*> layers = GetLayers();
	CreateInstanceCreateInfo(appInfo, extensions, &layers);
	EnumeratePhysicalDevices();
}

VulkanRender::~VulkanRender()
{

	for (auto& _swapchainBuffer : *_swapchainBuffers)
	{
		vkDestroyImageView(_device, *_swapchainBuffer.View(), nullptr);
	}
	vkDestroySwapchainKHR(_device, _swapChain, nullptr);

	VkCommandBuffer commandBuffers[1] = { _commandBuffer };
	vkFreeCommandBuffers(_device, _commandPool, 1, commandBuffers);
	vkDestroyCommandPool(_device, _commandPool, nullptr);
	vkDestroySurfaceKHR(_instance, _surface, nullptr);
	vkDestroyDevice(_device, nullptr);
	vkDestroyInstance(_instance, nullptr);	
}

VkFormat VulkanRender::GetSupportedFormat()
{
	uint32_t formatCount;
	VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
	VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(_gpus->at(0), _surface, &formatCount, nullptr);
	if (result != VK_SUCCESS)
		throw exception("unable to get device surface image formats count!");

	auto surfFormats = new VkSurfaceFormatKHR[formatCount];
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(_gpus->at(0), _surface, &formatCount, surfFormats);
	if (result != VK_SUCCESS)
		throw exception("unable to get device surface image formats!");

	if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED)
		format = VK_FORMAT_B8G8R8A8_UNORM;
	else
		format = surfFormats[0].format;

	return format;
}

void VulkanRender::InitSurface(int screenWidth, int screenHeight)
{
	InitDevice();
	CreateCommandPool(_queueInfo.queueFamilyIndex);
	CreateCommandBuffer();

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_gpus->at(0), _surface, &surfaceCapabilities);
	if(result != VK_SUCCESS)
	{
		throw exception("unable to get surface capabilities!");
	}

	uint32_t presentModeCount;
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(_gpus->at(0), _surface, &presentModeCount, nullptr);
	if (result != VK_SUCCESS)
	{
		throw exception("unable to get physical device present modes count!");
	}

	VkPresentModeKHR* presentModes = new VkPresentModeKHR[presentModeCount];
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(_gpus->at(0), _surface, &presentModeCount, presentModes);
	if (result != VK_SUCCESS)
	{
		throw exception("unable to get physical device present modes!");
	}

	if (surfaceCapabilities.currentExtent.width == INCORRECT_WIDTH)
	{
		_swapchainExtent.width = screenWidth;
		_swapchainExtent.height = screenHeight;

		if (_swapchainExtent.width < surfaceCapabilities.minImageExtent.width)
			_swapchainExtent.width = surfaceCapabilities.minImageExtent.width;
		else if (_swapchainExtent.width > surfaceCapabilities.maxImageExtent.width)
			_swapchainExtent.width = surfaceCapabilities.maxImageExtent.width;

		if (_swapchainExtent.height < surfaceCapabilities.minImageExtent.height)
			_swapchainExtent.height = surfaceCapabilities.minImageExtent.height;
		else if (_swapchainExtent.height > surfaceCapabilities.maxImageExtent.height)
			_swapchainExtent.height = surfaceCapabilities.maxImageExtent.height;
	}
	else
		_swapchainExtent = surfaceCapabilities.currentExtent;

	VkFormat supportedFormat = GetSupportedFormat();


	const VkSurfaceTransformFlagBitsKHR preTransform = GetPresentMode(surfaceCapabilities);
	const VkCompositeAlphaFlagBitsKHR compositeAlpha = GetCompositeAlpha(surfaceCapabilities);
	const VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	uint32_t desiredNumberOfSwapChainImages = surfaceCapabilities.minImageCount;

	if (surfaceCapabilities.maxImageCount > 0 && desiredNumberOfSwapChainImages > surfaceCapabilities.maxImageCount)
		desiredNumberOfSwapChainImages = surfaceCapabilities.maxImageCount;	

	const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkSwapchainCreateInfoKHR swapchain_ci = {};
	swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_ci.pNext = nullptr;
	swapchain_ci.surface = _surface;
	swapchain_ci.minImageCount = desiredNumberOfSwapChainImages;
	swapchain_ci.imageFormat = supportedFormat;
	swapchain_ci.imageExtent.width = _swapchainExtent.width;
	swapchain_ci.imageExtent.height = _swapchainExtent.height;
	swapchain_ci.preTransform = preTransform;
	swapchain_ci.compositeAlpha = compositeAlpha;
	swapchain_ci.imageArrayLayers = 1;
	swapchain_ci.presentMode = swapchainPresentMode;
	swapchain_ci.oldSwapchain = nullptr;
	swapchain_ci.clipped = true;
	swapchain_ci.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchain_ci.queueFamilyIndexCount = 0;
	swapchain_ci.pQueueFamilyIndices = nullptr;

	uint32_t queueFamilyIndices[2] = { _graphicsQueueFamilyIndex, _presentQueueFamilyIndex };
	if (_presentQueueFamilyIndex != _graphicsQueueFamilyIndex) 
	{
		swapchain_ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchain_ci.queueFamilyIndexCount = 2;
		swapchain_ci.pQueueFamilyIndices = queueFamilyIndices;
	}
	else 
	{
		swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchain_ci.queueFamilyIndexCount = 0; // Optional
		swapchain_ci.pQueueFamilyIndices = nullptr; // Optional
	}

	CreateSwapchainAndImages(swapchain_ci);
}

VkSurfaceTransformFlagBitsKHR VulkanRender::GetPresentMode(VkSurfaceCapabilitiesKHR surfaceCapabilities)
{	
	VkSurfaceTransformFlagBitsKHR preTransform;
	if ((surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) == 1)
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	else
		preTransform = surfaceCapabilities.currentTransform;

	return preTransform;
}

VkCompositeAlphaFlagBitsKHR VulkanRender::GetCompositeAlpha(VkSurfaceCapabilitiesKHR surfaceCapabilities)
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

void VulkanRender::CreateSwapchainAndImages(VkSwapchainCreateInfoKHR swapchainInfo)
{
	VkResult result = vkCreateSwapchainKHR(_device, &swapchainInfo, nullptr, &_swapChain);
	if (result != VK_SUCCESS)
	{
		throw exception("Unable to create swapchain!");
	}

	uint32_t imageCount;
	result = vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
	if (result != VK_SUCCESS)
	{
		throw exception("Unable to create swapchain!");
	}
	VkImage* swapchainImages = new VkImage[imageCount];
	result = vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, swapchainImages);
	if (result != VK_SUCCESS)
	{
		throw exception("Unable to create swapchain!");
	}

	_swapchainBuffers = new vector<SwapchainBuffer>(imageCount);
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

		VkResult resultImage = vkCreateImageView(_device, &color_image_view, nullptr, _swapchainBuffers->at(i).View());
		if(resultImage != VK_SUCCESS)
		{
			throw exception("Unable to create image!");
		}
	}
}

VkInstance VulkanRender::GetInstance() const
{
	return _instance;
}

VkSurfaceKHR* VulkanRender::GetSurface()
{
	return &_surface;
}

std::vector<const char*> VulkanRender::GetLayers()
{
	std::vector<const char*> layers;
#if defined(_DEBUG)
    layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif
	return layers;
}

VkApplicationInfo VulkanRender::CreateAppInfo()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = "App example";
	appInfo.applicationVersion = 1;
	appInfo.pEngineName = "";
	appInfo.engineVersion = 1;
	appInfo.apiVersion = VK_API_VERSION_1_1;

	return appInfo;
}

void VulkanRender::CreateInstanceCreateInfo(VkApplicationInfo appInfo, vector<const char*>* extensions, vector<const char*>* layers)
{
	VkInstanceCreateInfo instInfo = {};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pNext = nullptr;
    instInfo.flags = 0;
    instInfo.pApplicationInfo = &appInfo;
	instInfo.enabledExtensionCount = extensions->size();
	instInfo.ppEnabledExtensionNames = extensions->data();
	instInfo.enabledLayerCount = layers->size();
	instInfo.ppEnabledLayerNames = layers->data();

	const VkResult createResult = vkCreateInstance(&instInfo, nullptr, &_instance);
	if (createResult != VK_SUCCESS)
	{
		throw exception("Unable to create vulkan instance");
	}
}

void VulkanRender::CreateCommandPool(int queueFamilyIndex)
{
	VkCommandPoolCreateInfo commandPoolInfo;
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.pNext = nullptr;
	commandPoolInfo.queueFamilyIndex = queueFamilyIndex;
	commandPoolInfo.flags = 0;

	const VkResult result = vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_commandPool);
	if (result != VK_SUCCESS)
	{
		throw exception("Unable to create command pool!");
	}
}

bool VulkanRender::IsDeviceSuitable(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		deviceFeatures.geometryShader;
}

void VulkanRender::CreateCommandBuffer()
{
	VkCommandBufferAllocateInfo allocateInfo;
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.pNext = nullptr;
	allocateInfo.commandPool = _commandPool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandBufferCount = 1;

	const VkResult result = vkAllocateCommandBuffers(_device, &allocateInfo, &_commandBuffer);
	if (result != VK_SUCCESS)
	{
		throw exception("Unable to create command buffer!");
	}
}

void VulkanRender::EnumeratePhysicalDevices()
{	
	uint32_t gpu_count = 1;
	VkResult result = vkEnumeratePhysicalDevices(_instance, &gpu_count, nullptr);
	if(result != VK_SUCCESS)
	{
		throw exception("Unable to enumerate physical devices!");
	}	
	vector<VkPhysicalDevice>* suitableGpus = new vector<VkPhysicalDevice>(gpu_count);
	result = vkEnumeratePhysicalDevices(_instance, &gpu_count, suitableGpus->data());
	if(result != VK_SUCCESS)
	{
		throw exception("Unable to get physical devices!");
	}
	_gpus = new vector<VkPhysicalDevice>();
	for (auto& suitableGpu : *suitableGpus)
	{
		if(IsDeviceSuitable(suitableGpu))
		{
			_gpus->push_back(suitableGpu);
		}
	}
}

VkBool32* VulkanRender::GetQueuesSupportPresenting(uint32_t queueFamilyCount) const
{
	auto pSupportsPresent = new VkBool32[queueFamilyCount];
	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		vkGetPhysicalDeviceSurfaceSupportKHR(_gpus->at(0), i, _surface, &pSupportsPresent[i]);
	}
	return pSupportsPresent;
}

bool VulkanRender::GetGraphicsAndPresentQueue(uint32_t queueFamilyCount, vector<VkQueueFamilyProperties> queueProps)
{
	VkBool32* pSupportsPresent = new VkBool32[queueFamilyCount];
	for (uint32_t i = 0; i < queueFamilyCount; i++) 
	{
		vkGetPhysicalDeviceSurfaceSupportKHR(_gpus->at(0), i, _surface, &pSupportsPresent[i]);
	}

	bool found = false;
	for (unsigned int i = 0; i < queueFamilyCount; i++)
	{
		if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
			continue;
		
		if (_graphicsQueueFamilyIndex == UINT32_MAX)
			_queueInfo.queueFamilyIndex = i;
		
		if (pSupportsPresent[i] == VK_TRUE) 
		{
			_graphicsQueueFamilyIndex = i;
			_presentQueueFamilyIndex = i;
			break;
		}

		found = true;
		break;
	}
	return found;
}

void VulkanRender::InitDevice()
{
	_queueInfo = {};

	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(_gpus->at(0), &queueFamilyCount, nullptr);

	vector<VkQueueFamilyProperties> queueProps(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(_gpus->at(0), &queueFamilyCount, queueProps.data());

	bool found = GetGraphicsAndPresentQueue(queueFamilyCount, queueProps);

	float queue_priorities[1] = { 0.0 };
	_queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	_queueInfo.pNext = nullptr;
	_queueInfo.queueCount = 1;
	_queueInfo.pQueuePriorities = queue_priorities;

	_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	VkDeviceCreateInfo device_info = {};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext = nullptr;
	device_info.queueCreateInfoCount = 1;
	device_info.pQueueCreateInfos = &_queueInfo;
	device_info.enabledExtensionCount = _extensions.size();
	device_info.ppEnabledExtensionNames = _extensions.data();
	device_info.pEnabledFeatures = nullptr;

	const VkResult result = vkCreateDevice(_gpus->at(0), &device_info, nullptr, &_device);
	if (result != VK_SUCCESS)
	{
		throw exception("Unable to create physical device!");
	}	
}
