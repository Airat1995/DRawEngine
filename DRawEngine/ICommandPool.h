#pragma once
#include <exception>
#include "ICommandBuffer.h"
#include <vector>

using namespace std;

class ICommandPool
{
public:
	ICommandPool(VkDevice* device, int queueFamilyIndex);

	virtual ~ICommandPool();

	void AddCommandBuffer();

	VkCommandPool* CommandPool();

private:
	VkCommandPool _commandPool;

	VkDevice* _device;

	vector<VkCommandBuffer> _commandBuffers;
};

