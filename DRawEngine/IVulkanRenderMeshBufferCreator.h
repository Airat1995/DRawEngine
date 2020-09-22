#pragma once
#include "IMesh.h"
#include "VulkanBuffer.h"

class IVulkanRenderMeshBufferCreator
{
public:
	virtual void ReCreateBuffers(IMesh* mesh, vector<VulkanBuffer> perObjectBuffers) = 0;
};
