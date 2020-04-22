#include "VulkanFramebuffer.h"

VulkanFramebuffer::VulkanFramebuffer(VkDevice device, uint32_t grapQueueFI, uint32_t presentQueueFI, ISwapchain& swapchain,
                           VulkanRenderpass& renderpass, VulkanCommandPool& commandPool) : _device(device), _commandPool(commandPool),
                                                                             _swapchain(swapchain), _renderpass(renderpass)
{
	CreateQueues(device, grapQueueFI, presentQueueFI);

	vector<SwapchainBuffer> swapchainBuffers = _swapchain.SwapchainBuffers();
	_swapChainFramebuffers = new vector<VkFramebuffer>(0);
	for (auto swapchainBuffer : swapchainBuffers)
	{
		VkImageView attachments[] = {
			*swapchainBuffer.View()
		};

		VkFramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = _renderpass.RenderPass();
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = _swapchain.SwapchainInfo().imageExtent.width;
		framebufferCreateInfo.height = _swapchain.SwapchainInfo().imageExtent.height;
		framebufferCreateInfo.layers = 1;
		framebufferCreateInfo.pNext = nullptr;
		framebufferCreateInfo.flags = 0;

		VkFramebuffer framebuffer;
		VkResult result = vkCreateFramebuffer(_device, &framebufferCreateInfo, nullptr, &framebuffer);
		if (result != VK_SUCCESS)
			throw runtime_error("Unable to create framebuffer");
		_swapChainFramebuffers->push_back(framebuffer);
	}
}

VkFramebuffer* VulkanFramebuffer::Framebuffer(int index)
{
	return &_swapChainFramebuffers->at(index);
}

int VulkanFramebuffer::FramebufferCount() const
{
	return _swapChainFramebuffers->size();
}

VulkanFramebuffer::~VulkanFramebuffer()
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

void VulkanFramebuffer::CreateQueues(VkDevice device, uint32_t grapQueueFI, uint32_t presentQueueFI)
{
	vkGetDeviceQueue(device, grapQueueFI, 0, &_drawQueue);
	vkGetDeviceQueue(device, presentQueueFI, 0, &_presentQueue);
}

void VulkanFramebuffer::DrawFrame()
{
	uint32_t imageIndex;
	auto result = vkAcquireNextImageKHR(_device, _swapchain.Swapchain(), UINT64_MAX, _swapchain.ImageAvailableSemaphore(), nullptr, &imageIndex);
	if (result != VK_SUCCESS)
	{
		cerr << "Unable to acquire image from framebuffer";
	}
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkFence drawFence;
	VkFenceCreateInfo fenceInfo;
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.pNext = nullptr;
	fenceInfo.flags = 0;
	VkResult res = vkCreateFence(_device, &fenceInfo, nullptr, &drawFence);
	if (res != VK_SUCCESS)
		throw exception("Unable to create fence!");

	VkSemaphore waitSemaphores[] = { _swapchain.ImageAvailableSemaphore() };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = _commandPool.CommandBufferCount();
	submitInfo.pCommandBuffers = _commandPool.CommandBuffersData();
	submitInfo.pSignalSemaphores = nullptr;
	submitInfo.pNext = nullptr;
	VkSemaphore signalSemaphores[] = { _swapchain.RenderFinishSemaphore() };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(_drawQueue, 1, &submitInfo, nullptr) != VK_SUCCESS)
		throw std::runtime_error("failed to submit draw command buffer!");
	
	//do {
	//	res = vkWaitForFences(_device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
	//} while (res == VK_TIMEOUT);

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { _swapchain.Swapchain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;
	res = vkQueuePresentKHR(_presentQueue, &presentInfo);
	if (res == VK_ERROR_OUT_OF_DATE_KHR)
		_swapchain.RecreateSwapchain();
	else if (res != VK_SUCCESS)
		throw exception("Unable to get image from queue!");	
}
