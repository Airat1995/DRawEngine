#pragma once
#include <vulkan/vulkan.h>
#include "ISwapchain.h"
#include "IPipeline.h"

using namespace std;

class IFramebuffer
{
public:
	explicit IFramebuffer(VkDevice* device, IPipeline* pipeline, ISwapchain* swapchain);

	virtual ~IFramebuffer();
private:
	vector<VkFramebuffer>* _swapChainFramebuffers;

	VkDevice* _device;
};

