#pragma once

#include <vulkan/vulkan_core.h>
#include <stdexcept>
#include "ISwapchain.h"

class IPipeline
{
public:
	IPipeline(VkDevice* device, ISwapchain* swapchain, VkExtent2D* extent);

	VkRenderPass* RenderPass();

	virtual ~IPipeline();
private:
	VkDynamicState _dynamicStates[2] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineLayout _pipelineLayout;

	VkDevice* _device;

	VkRenderPass _renderPass;
};

