#pragma once
#include <vulkan/vulkan.h>
#include "ISwapchain.h"
#include "IPipeline.h"
#include "ICommandPool.h"
#include "IShader.h"

using namespace std;

class IFramebuffer
{
public:
	void CreateQueues(VkDevice device, uint32_t graphicsQueueFamilyIndex, uint32_t presentQueueFamilyIndex);
	explicit IFramebuffer(VkDevice device, vector<IShader>& shaders, ICommandPool& commandPool, VkExtent2D swapchainExtent, VkSurfaceCapabilitiesKHR surfaceCapabilities,
		VkSurfaceKHR surface, vector<VkPhysicalDevice>& gpus, uint32_t graphicsQueueFamilyIndex,
		uint32_t presentQueueFamilyIndex);

	VkFramebuffer* Framebuffer(int index);

	void BeginRenderPass(int index, ICommandBuffer& commandBuffer);

	virtual ~IFramebuffer();
	
	void DrawFrame();
private:
	vector<VkFramebuffer>* _swapChainFramebuffers;

	vector<SwapchainBuffer>* _swapchainBuffers;

	VkDevice _device;

	ISwapchain* _swapchain;

	IPipeline* _pipeline;

	VkQueue _drawQueue;

	VkQueue _presentQueue;
	
	ICommandPool& _commandPool;
};
