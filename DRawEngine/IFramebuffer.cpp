#include "IFramebuffer.h"

IFramebuffer::IFramebuffer(VkDevice device, vector<IShader>& shaders, ICommandPool& commandPool, VkExtent2D swapchainExtent, VkSurfaceCapabilitiesKHR surfaceCapabilities,
	VkSurfaceKHR surface, vector<VkPhysicalDevice>& gpus, uint32_t graphicsQueueFamilyIndex,
	uint32_t presentQueueFamilyIndex) :_device(device), _commandPool(commandPool)
{

	CreateQueues(device, graphicsQueueFamilyIndex, presentQueueFamilyIndex);

	_swapchain = new ISwapchain(_device, swapchainExtent, surfaceCapabilities, surface, gpus, graphicsQueueFamilyIndex, presentQueueFamilyIndex);
	for (auto swapchain : _swapchain->SwapchainBuffers())
		commandPool.AddCommandBuffer();

	_pipeline = new IPipeline(_device, shaders, _swapchain->SwapchainInfo()->imageFormat, &swapchainExtent);
	vector<SwapchainBuffer> swapchainBuffers = _swapchain->SwapchainBuffers();
	_swapChainFramebuffers = new vector<VkFramebuffer>(0);
	for (auto swapchainBuffer : swapchainBuffers)
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
		framebufferCreateInfo.pNext = nullptr;
		framebufferCreateInfo.flags = 0;

		VkFramebuffer framebuffer;
		VkResult result = vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffer);
		if (result != VK_SUCCESS)
			throw runtime_error("Unable to create framebuffer");
		_swapChainFramebuffers->push_back(framebuffer);
	}

	int commandBufferCount = commandPool.CommandBufferCount();
	for (size_t i = 0; i < commandBufferCount; i++)
	{
		BeginRenderPass(i, commandPool.CommandBuffer(i));
	}
	
}

VkFramebuffer* IFramebuffer::Framebuffer(int index)
{
	return &_swapChainFramebuffers->at(index);
}

void IFramebuffer::BeginRenderPass(int index, ICommandBuffer& commandBuffer)
{
	VkRenderPassBeginInfo renderPassBeginInfo;
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = *_pipeline->RenderPass();
	renderPassBeginInfo.framebuffer = *Framebuffer(index);
	renderPassBeginInfo.renderArea.offset = { 0,0 };
	renderPassBeginInfo.renderArea.extent = _swapchain->SwapchainInfo()->imageExtent;
	renderPassBeginInfo.pNext = nullptr;
	VkClearValue clearColor = { 0.0f, 0.0f, 0.7f, 1.0f };
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearColor;
	commandBuffer.BeginCommandBuffer();
	vkCmdBeginRenderPass(commandBuffer.CommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer.CommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline->Pipeline());
	//vkCmdBindPipeline(*commandBuffer->CommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, );
	vkCmdDraw(commandBuffer.CommandBuffer(), 3, 1, 0, 0);
	vkCmdEndRenderPass(commandBuffer.CommandBuffer());
	if (vkEndCommandBuffer(commandBuffer.CommandBuffer()) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

IFramebuffer::~IFramebuffer()
{
	for (auto& _swapchainBuffer : *_swapchainBuffers)
	{
		vkDestroyImageView(_device, *_swapchainBuffer.View(), nullptr);
	}

	for (auto framebuffer : *_swapChainFramebuffers)
	{
		vkDestroyFramebuffer(_device, framebuffer, nullptr);
	}
}

void IFramebuffer::CreateQueues(VkDevice device, uint32_t graphicsQueueFamilyIndex, uint32_t presentQueueFamilyIndex)
{
	vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &_drawQueue);
	vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &_presentQueue);
}

void IFramebuffer::DrawFrame()
{
	uint32_t imageIndex;
	auto result = vkAcquireNextImageKHR(_device, _swapchain->Swapchain(), UINT64_MAX, _swapchain->ImageAvailableSemaphore(), nullptr, &imageIndex);
	if (result != VK_SUCCESS)
	{
		cerr << "Unable to acquire image from framebuffer";
	}
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { _swapchain->ImageAvailableSemaphore() };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_commandPool.CommandBuffer(imageIndex).CommandBuffer();
	submitInfo.pSignalSemaphores = nullptr;
	submitInfo.pNext = nullptr;
	VkSemaphore signalSemaphores[] = { _swapchain->RenderFinishSemaphore() };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(_drawQueue, 1, &submitInfo, nullptr) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { _swapchain->Swapchain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;
	vkQueuePresentKHR(_presentQueue, &presentInfo);
}
