#include "VulkanRender.h"
#include <cassert>
#include "IBuffer.h"


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
	delete(_swapchain);
	for (auto& _swapchainBuffer : *_swapchainBuffers)
	{
		vkDestroyImageView(_device, *_swapchainBuffer.View(), nullptr);
	}	

	VkCommandBuffer commandBuffers[1] = { _commandBuffer };
	vkFreeCommandBuffers(_device, _commandPool, 1, commandBuffers);
	vkDestroyCommandPool(_device, _commandPool, nullptr);
	vkDestroySurfaceKHR(_instance, _surface, nullptr);
	vkDestroyDevice(_device, nullptr);
	vkDestroyInstance(_instance, nullptr);	
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

	_swapchain = new ISwapchain(&_device, _swapchainExtent, surfaceCapabilities, _surface, _gpus, _graphicsQueueFamilyIndex, _presentQueueFamilyIndex);
}


VkInstance VulkanRender::GetInstance() const
{
	return _instance;
}

VkSurfaceKHR* VulkanRender::GetSurface()
{
	return &_surface;
}

void VulkanRender::CreateDepthBuffer()
{
	_depthBuffer = new IImage(VK_FORMAT_D16_UNORM, &_device, _gpus, VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
	                          _swapchainExtent.width, _swapchainExtent.height, VK_SAMPLE_COUNT_1_BIT);	
}

void VulkanRender::CreateBuffer()
{
	return;
}

vector<const char*> VulkanRender::GetLayers()
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
