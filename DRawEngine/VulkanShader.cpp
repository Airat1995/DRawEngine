#include "VulkanShader.h"

VulkanShader::VulkanShader(VkDevice device, IShader& basicShader) : IShader(basicShader.ShaderLocation(), basicShader.Name(), basicShader.GetShaderType()), _device(device)
{
	const bool isShaderCreated = CreateShaderModule(device, basicShader.ShaderData());
	if (isShaderCreated)
		CreatePipelineShaderStageCreateInfo(basicShader.GetShaderType(), basicShader.Name());
	else
		cerr << "Unable to create shader module. Shader location: " << basicShader.ShaderLocation().c_str() << endl;

}

VulkanShader::VulkanShader(VkDevice device, ShaderType shaderType, string& shaderLocation, string& name) : IShader(shaderLocation, name, shaderType), _device(device)
{
	const bool isShaderCreated = CreateShaderModule(device, _shaderData);
	FileReader reader = FileReader();
	if (isShaderCreated)
		CreatePipelineShaderStageCreateInfo(shaderType, name);
	else
		cerr << "Unable to create shader module. Shader location: " << shaderLocation.c_str() << endl;
}

VulkanShader::~VulkanShader()
{	
	_shaderModule = nullptr;
	
}

void VulkanShader::DestroyShader()
{
	vkDestroyShaderModule(_device, _shaderModule, nullptr);
}


VkPipelineShaderStageCreateInfo VulkanShader::GetShaderStageInfo()
{
	return _pipelineShader;
}

void VulkanShader::CreatePipelineShaderStageCreateInfo(ShaderType shaderType, string& moduleName)
{
	_pipelineShader.flags = 0;
	_pipelineShader.pNext = nullptr;
	_pipelineShader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	_pipelineShader.stage = VkShaderStageFlagBits(static_cast<int32_t>(shaderType));
	_pipelineShader.module = _shaderModule;
	_pipelineShader.pSpecializationInfo = nullptr;
	_pipelineShader.pName = "main";
}

bool VulkanShader::CreateShaderModule(VkDevice device, vector<char>& fileData)
{
	VkShaderModuleCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = fileData.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(fileData.data());
	createInfo.pNext = nullptr;	
	createInfo.flags = 0;

	VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &_shaderModule);
	return result == VK_SUCCESS;
}
