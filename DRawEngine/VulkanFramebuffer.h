#pragma once
#include <vulkan/vulkan.h>
#include "IFramebuffer.h"
#include "ISwapchain.h"
#include "VulkanPipeline.h"
#include "VulkanCommandPool.h"
#include "IBuffer.h"


using namespace std;

class VulkanFramebuffer : public IFramebuffer
{
public:
	VulkanFramebuffer(VkDevice device, uint32_t grapQueueFI, uint32_t presentQueueFI, ISwapchain& swapchain,
		VulkanRenderpass& renderpass, VulkanCommandPool& commandPool);
	

	VkFramebuffer* Framebuffer(int index);
	
	int FramebufferCount() const;

	void CreateQueues(VkDevice device, uint32_t grapQueueFI, uint32_t presentQueueFI);

	virtual ~VulkanFramebuffer();
	
	void DrawFrame();
private:
	vector<VkFramebuffer>* _swapChainFramebuffers;

	vector<SwapchainBuffer>* _swapchainBuffers;

	VkDevice _device;

	VkQueue _drawQueue;

	VkQueue _presentQueue;

	ISwapchain& _swapchain;

	VulkanCommandPool& _commandPool;

	VulkanRenderpass& _renderpass;
};

