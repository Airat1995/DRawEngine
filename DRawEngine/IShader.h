#pragma once
#include <vulkan/vulkan_core.h>
#include <iostream>
#include "FileReader.h"

using namespace  std;

enum class ShaderType
{
	Vertex = 0x00000001,
	TesselationControl = 0x00000002,
	TesselationEvaluation = 0x00000004,
	Geometry = 0x00000008,
	Fragment = 0x00000010,
	Compute = 0x00000020,
	Raygen = 0x00000100,
	AnyHit = 0x00000200,
	ClosetHit = 0x00000400,
	Mesh = 0x00000080,
};


class IShader
{
public:	
	explicit IShader(VkDevice* device, ShaderType shaderType, string* shaderLocation, string* name = nullptr);

	virtual ~IShader();

	VkPipelineShaderStageCreateInfo* GetShaderStageInfo();

protected:
	VkShaderModule _shaderModule{};

	VkPipelineShaderStageCreateInfo _pipelineShader{};
		
	VkDevice* _device;

private:
	void CreatePipelineShaderStageCreateInfo(ShaderType shaderType, string* moduleName);

	bool CreateShaderModule(VkDevice* device, string* fileData);
};
