#pragma once
#include "IMesh.h"

class IVulkanRenderMeshBufferCreator
{
public:
	virtual void ReCreateBuffers(IMesh* mesh) = 0;
};
