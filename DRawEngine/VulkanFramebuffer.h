#pragma once
#include <vulkan/vulkan.h>
#include "IFramebuffer.h"
#include "ISwapchain.h"
#include "VulkanPipeline.h"
#include "VulkanCommandPool.h"
#include "IBuffer.h"
#include "VulkanDepthBuffer.h"


using namespace std;

class VulkanFramebuffer : public IFramebuffer
{
public:
	VulkanFramebuffer(VkDevice device, uint32_t grapQueueFI, uint32_t presentQueueFI, ISwapchain& swapchain,
		VulkanRenderpass& renderpass, VulkanCommandPool& commandPool, VulkanDepthBuffer& depthBuffer);

	VulkanFramebuffer(VkDevice device, uint32_t grapQueueFI, uint32_t presentQueueFI, ISwapchain& swapchain,
		BufferlessVulkanImage& image, VulkanRenderpass& renderpass, VulkanCommandPool& commandPool,
		VulkanDepthBuffer& depthBuffer);


	VkFramebuffer* Framebuffer(int index);
	
	int FramebufferCount() const;

	void CreateQueues(VkDevice device, uint32_t grapQueueFI, uint32_t presentQueueFI);

	virtual ~VulkanFramebuffer();
	
	void SubmitFramebuffer(int index);

	VkQueue PresentQueue();

	VkQueue DrawQueue();

private:
	vector<VkFramebuffer>* _swapChainFramebuffers;

	vector<SwapchainBuffer>* _swapchainBuffers;

	VkDevice _device;

	VkQueue _drawQueue;

	VkQueue _presentQueue;

	ISwapchain& _swapchain;

	VulkanCommandPool& _commandPool;

	VulkanRenderpass& _renderpass;

	VkSemaphore presentCompleteSemaphore;
	
	VkSemaphore renderCompleteSemaphore;

	std::vector<VkFence> waitFences;

	PFN_vkQueuePresentKHR fpQueuePresentKHR;
};

