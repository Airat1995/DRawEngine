#pragma once

#include <vulkan/vulkan_core.h>
#include <stdexcept>
#include <vector>
#include "IShader.h"

using namespace std;

class IPipeline
{
public:
	IPipeline(VkDevice device, vector<IShader>& shaders, VkFormat imageFormat, VkExtent2D* extent);

	VkRenderPass* RenderPass();

	virtual ~IPipeline();
private:
	VkDynamicState _dynamicStates[2] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineLayout _pipelineLayout;

	VkDevice _device;
	
	VkPipeline _pipeline;

	VkRenderPass _renderPass;

};

