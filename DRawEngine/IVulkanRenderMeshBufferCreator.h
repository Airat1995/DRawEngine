#pragma once
#include "IMesh.h"
#include "VulkanBuffer.h"

class IVulkanRenderMeshBufferCreator
{
public:
	virtual void AddMesh(IMesh* mesh, vector<VulkanBuffer> perObjectBuffers) = 0;
};
