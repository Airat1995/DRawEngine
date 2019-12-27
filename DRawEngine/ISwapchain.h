#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include "SwapchainBuffer.h"


using namespace std;

class ISwapchain
{
public:
	ISwapchain(VkDevice* device, VkExtent2D swapchainExtent, VkSurfaceCapabilitiesKHR surfaceCapabilities,
	           VkSurfaceKHR surface, vector<VkPhysicalDevice>* gpus, uint32_t graphicsQueueFamilyIndex,
	           uint32_t presentQueueFamilyIndex);

	~ISwapchain();

protected:
	void CreateSwapchainAndImages(VkDevice* device, VkSwapchainCreateInfoKHR swapchainInfo);

	VkSurfaceTransformFlagBitsKHR GetPresentMode(VkSurfaceCapabilitiesKHR surfaceCapabilities);

	VkCompositeAlphaFlagBitsKHR GetCompositeAlpha(VkSurfaceCapabilitiesKHR surfaceCapabilities);

	VkFormat GetSupportedFormat(vector<VkPhysicalDevice>* gpus, VkSurfaceKHR surface);

	void CreateSemaphore();

	std::vector<SwapchainBuffer>* _swapchainBuffers = nullptr;

	VkSwapchainKHR _swapChain = nullptr;

	VkDevice* _device;

	VkSemaphore _imageAvailableSemaphore;

	VkSemaphore _renderFinishSemaphore;
	
};

