#pragma once
#include "IBuffer.h"
#include "Mesh.h"
#include "IVertex.h"

class VertexBuffer : public IBuffer<IVertex>
{
public:

	VertexBuffer(VkDevice* device, VkPhysicalDevice physical, BufferUsageFlag usage, SharingMode sharingMode,
		Mesh& mesh)
		: IBuffer<IVertex>(device, physical, usage, sharingMode, nullptr),
		  _mesh(mesh)
	{
		_bufferInfo = {};
		_bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		_bufferInfo.pNext = nullptr;
		_bufferInfo.usage = static_cast<VkBufferUsageFlags>(usage);
		_bufferInfo.size = mesh.RequiredBufferSize();
		_bufferInfo.queueFamilyIndexCount = 0;
		_bufferInfo.pQueueFamilyIndices = nullptr;
		_bufferInfo.sharingMode = static_cast<VkSharingMode>(sharingMode);
		_bufferInfo.flags = 0;
		VkResult result = vkCreateBuffer(*device, &_bufferInfo, nullptr, &_buffer);
		if (result != VK_SUCCESS)
			cerr << "Unable to create buffer!" << endl;

		vkGetBufferMemoryRequirements(*device, _buffer, &_memoryReq);
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physical, &memProperties);

		VkMemoryAllocateInfo allocInfo;
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;
		allocInfo.memoryTypeIndex = FindMemoryType(_memoryReq.memoryTypeBits, memProperties,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		allocInfo.allocationSize = _memoryReq.size;

		result = vkAllocateMemory(*device, &allocInfo, nullptr, &_memory);
		if (result != VK_SUCCESS)
		{
			cerr << "Unable to allocate memory!!" << endl;
			return;
		}
		result = vkBindBufferMemory(*device, _buffer, _memory, 0);
		if (result != VK_SUCCESS)
		{
			cerr << "Unable to bind memory!" << endl;
			return;
		}
	}

	void Fill() override
	{
		VkResult result = vkMapMemory(*_device, _memory, 0, _memoryReq.size, 0, reinterpret_cast<void**>(&_dataPointer));
		if (result != VK_SUCCESS)
			cerr << "Unable to map memory!" << endl;
		memcpy(_dataPointer, _mesh.VerticesData(), _bufferInfo.size);
		vkUnmapMemory(*_device, _memory);

		_bufferDescriptorInfo.buffer = _buffer;
		_bufferDescriptorInfo.offset = 0;
		_bufferDescriptorInfo.range = _mesh.VertexSize();

	}

	vector<VkVertexInputBindingDescription>& BindingDescriptions()
	{
		return _mesh.BindingDescriptions();
	}

	vector<VkVertexInputAttributeDescription>& AttributeDescriptions()
	{
		return _mesh.AttributeDescriptions();
	}


protected:

	Mesh& _mesh;
};
