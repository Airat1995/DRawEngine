#include "IFramebuffer.h"

IFramebuffer::IFramebuffer(VkDevice* device, vector<IShader>* shaders, ICommandPool* commandPool, VkExtent2D swapchainExtent, VkSurfaceCapabilitiesKHR surfaceCapabilities,
	VkSurfaceKHR surface, vector<VkPhysicalDevice>* gpus, uint32_t graphicsQueueFamilyIndex,
	uint32_t presentQueueFamilyIndex) :_device(device)
{
	_swapchain = new ISwapchain(_device, swapchainExtent, surfaceCapabilities, surface, gpus, graphicsQueueFamilyIndex, presentQueueFamilyIndex);
	for (auto swapchain : *_swapchain->SwapchainBuffers())
		commandPool->AddCommandBuffer();

	_pipeline = new IPipeline(_device, shaders, _swapchain->SwapchainInfo()->imageFormat, &swapchainExtent);
	vector<SwapchainBuffer>* swapchainBuffers = _swapchain->SwapchainBuffers();
	_swapChainFramebuffers = new vector<VkFramebuffer>(0);
	for (auto swapchainBuffer : *swapchainBuffers)
	{
		VkImageView attachments[] = {
			*swapchainBuffer.View()
		};

		VkFramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = *_pipeline->RenderPass();
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = _swapchain->SwapchainInfo()->imageExtent.width;
		framebufferCreateInfo.height = _swapchain->SwapchainInfo()->imageExtent.height;
		framebufferCreateInfo.layers = 1;

		VkFramebuffer framebuffer;
		VkResult result = vkCreateFramebuffer(*device, &framebufferCreateInfo, nullptr, &framebuffer);
		if (result != VK_SUCCESS)
			throw runtime_error("Unable to create framebuffer");
		_swapChainFramebuffers->push_back(framebuffer);
	}
}

VkFramebuffer* IFramebuffer::Framebuffer(int index)
{
	return &_swapChainFramebuffers->at(index);
}

void IFramebuffer::BeginRenderPass(int index, ICommandBuffer* commandBuffer)
{
	VkRenderPassBeginInfo renderPassBeginInfo;
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = *_pipeline->RenderPass();
	renderPassBeginInfo.framebuffer = *Framebuffer(index);
	renderPassBeginInfo.renderArea.offset = { 0,0 };
	renderPassBeginInfo.renderArea.extent = _swapchain->SwapchainInfo()->imageExtent;
	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearColor;
	vkCmdBeginRenderPass(*commandBuffer->CommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	//vkCmdBindPipeline(*commandBuffer->CommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, );

}

IFramebuffer::~IFramebuffer()
{
	for (auto& _swapchainBuffer : *_swapchainBuffers)
	{
		vkDestroyImageView(*_device, *_swapchainBuffer.View(), nullptr);
	}

	for (auto framebuffer : *_swapChainFramebuffers)
	{
		vkDestroyFramebuffer(*_device, framebuffer, nullptr);
	}
}
