#include "IFramebuffer.h"

IFramebuffer::IFramebuffer(VkDevice* device, IPipeline* pipeline, ISwapchain* swapchain) :_device(device)
{
	vector<SwapchainBuffer>* swapchainBuffers = swapchain->SwapchainBuffers();
	_swapChainFramebuffers = new vector<VkFramebuffer>(0);
	for (auto swapchainBuffer : *swapchainBuffers)
	{
		VkImageView attachments[] = {
			*swapchainBuffer.View()
		};

		VkFramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = *pipeline->RenderPass();
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = swapchain->SwapchainInfo()->imageExtent.width;
		framebufferCreateInfo.height = swapchain->SwapchainInfo()->imageExtent.height;
		framebufferCreateInfo.layers = 1;

		VkFramebuffer framebuffer;
		VkResult result = vkCreateFramebuffer(*device, &framebufferCreateInfo, nullptr, &framebuffer);
		if (result != VK_SUCCESS)
			throw runtime_error("Unable to create framebuffer");
		else
			_swapChainFramebuffers->push_back(framebuffer);
	}
}

IFramebuffer::~IFramebuffer()
{
	for (auto framebuffer : *_swapChainFramebuffers)
	{
		vkDestroyFramebuffer(*_device, framebuffer, nullptr);
	}
}
