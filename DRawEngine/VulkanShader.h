#pragma once
#include <vulkan/vulkan_core.h>
#include <iostream>
#include "IShader.h"

using namespace std;

class VulkanShader : public IShader
{
public:

	VulkanShader(VkDevice device, IShader& basicShader);
	
	explicit VulkanShader(VkDevice device, ShaderType shaderType, string& shaderLocation, string& name);

	virtual ~VulkanShader();

	void DestroyShader();

	VkPipelineShaderStageCreateInfo GetShaderStageInfo();

protected:
	VkShaderModule _shaderModule{};

	VkPipelineShaderStageCreateInfo _pipelineShader{};
		
	VkDevice _device;

private:
	void CreatePipelineShaderStageCreateInfo(ShaderType shaderType, string& moduleName);

	bool CreateShaderModule(VkDevice device, vector<char>& fileData);
};
