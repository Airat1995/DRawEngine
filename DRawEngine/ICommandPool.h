#pragma once
#include <exception>
#include "ICommandBuffer.h"
#include <vector>

using namespace std;

class ICommandPool
{
public:
	ICommandPool(VkDevice device, int queueFamilyIndex);
	
	virtual ~ICommandPool();

	void AddCommandBuffer();

	void BeginCommandPool();

	int CommandBufferCount();

	ICommandBuffer& CommandBuffer(int index);

	VkCommandPool* CommandPool();

private:
	VkCommandPool _commandPool;

	VkDevice _device;

	vector<ICommandBuffer> _commandBuffers;
};
