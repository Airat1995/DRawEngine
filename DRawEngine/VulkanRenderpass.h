#pragma once

#include <vulkan/vulkan_core.h>
#include "IRenderpass.h"

class VulkanRenderpass :
	public IRenderpass
{
public:
	VulkanRenderpass(VkDevice device, VkFormat colorFormat, VkFormat depthFormat);

	VkRenderPass RenderPass();

	void BeginRenderPass(int width, int height, VkFramebuffer framebuffer, VkCommandBuffer commandBuffer);

	void EndRenderPass(VkCommandBuffer commandBuffer);
protected:

	VkRenderPass _renderPass;

	VkRenderPassCreateInfo _renderPassCreateInfo;

	VkRenderPassBeginInfo _renderPassBeginInfo;
private:

	inline VkRenderPassBeginInfo RenderPassBeginInfo()
	{
		
	}
};

