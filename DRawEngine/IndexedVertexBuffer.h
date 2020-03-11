#pragma once
#include "VertexBuffer.h"

class IndexedVertexBuffer : public VertexBuffer
{
public:
	IndexedVertexBuffer(VkDevice* device, VkPhysicalDevice physical, BufferUsageFlag usage, SharingMode sharingMode,
		Mesh& mesh)
		: VertexBuffer(device, physical, usage, sharingMode, mesh)
	{
		_indexBufferInfo = {};
		_indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		_indexBufferInfo.pNext = nullptr;
		_indexBufferInfo.usage = static_cast<VkBufferUsageFlags>(usage);
		_indexBufferInfo.size = mesh.RequiredBufferSize();
		_indexBufferInfo.queueFamilyIndexCount = 0;
		_indexBufferInfo.pQueueFamilyIndices = nullptr;
		_indexBufferInfo.sharingMode = static_cast<VkSharingMode>(sharingMode);
		_indexBufferInfo.flags = 0;
		VkResult result = vkCreateBuffer(*device, &_indexBufferInfo, nullptr, &_indexesBuffer);
		if (result != VK_SUCCESS)
			cerr << "Unable to create buffer!" << endl;

		vkGetBufferMemoryRequirements(*device, _indexesBuffer, &_indexMemoryReq);
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physical, &memProperties);

		VkMemoryAllocateInfo allocInfo;
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;
		allocInfo.memoryTypeIndex = FindMemoryType(_indexMemoryReq.memoryTypeBits, memProperties,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		allocInfo.allocationSize = _indexMemoryReq.size;

		result = vkAllocateMemory(*device, &allocInfo, nullptr, &_indexesMemory);
		if (result != VK_SUCCESS)
		{
			cerr << "Unable to allocate memory!!" << endl;
			return;
		}
		result = vkBindBufferMemory(*device, _indexesBuffer, _indexesMemory, 0);
		if (result != VK_SUCCESS)
		{
			cerr << "Unable to bind memory!" << endl;
			return;
		}
	}

	void Fill() override
	{
		VertexBuffer::Fill();

		VkResult result = vkMapMemory(*_device, _indexesMemory, 0, _memoryReq.size, 0, reinterpret_cast<void**>(&_indexDataPointer));
		if (result != VK_SUCCESS)
			cerr << "Unable to map memory!" << endl;
		memcpy(_indexDataPointer, _mesh.IndicesData(), _bufferInfo.size);
		vkUnmapMemory(*_device, _indexesMemory);

		_bufferDescriptorInfo.buffer = _indexesBuffer;
		_bufferDescriptorInfo.offset = _mesh.VertexSize();
		_bufferDescriptorInfo.range = _mesh.IndexSize();

	}

	VkBuffer& IndexedBuffer()
	{
		return _indexesBuffer;
	}

	int IndexesCount()
	{
		return _mesh.IndexSize();
	}

protected:

	uint8_t* _indexDataPointer = nullptr;

	VkBuffer _indexesBuffer{};

	VkDeviceMemory _indexesMemory{};

	VkBufferCreateInfo _indexBufferInfo{};

	VkMemoryRequirements _indexMemoryReq{};
};
