#include "IShader.h"

IShader::IShader(VkDevice* device, ShaderType shaderType, string* shaderLocation, string* name) : _device(device)
{
	FileReader reader = FileReader();
	string* fileData = reader.Read(shaderLocation);
	const bool isShaderCreated = CreateShaderModule(device, fileData);
	if (isShaderCreated)
		CreatePipelineShaderStageCreateInfo(shaderType, name);
	else
		cerr << "Unable to create shader module. Shader location: " << shaderLocation->c_str() << endl;
}

IShader::~IShader()
{
	vkDestroyShaderModule(*_device, _shaderModule, nullptr);
}

VkPipelineShaderStageCreateInfo* IShader::GetShaderStageInfo()
{
	return &_pipelineShader;
}

void IShader::CreatePipelineShaderStageCreateInfo(ShaderType shaderType, string* moduleName)
{
	_pipelineShader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	_pipelineShader.stage = VkShaderStageFlagBits(static_cast<int32_t>(shaderType));
	_pipelineShader.module = _shaderModule;
	_pipelineShader.pName = moduleName != nullptr ? moduleName->c_str() : "";
}

bool IShader::CreateShaderModule(VkDevice* device, string* fileData)
{
	VkShaderModuleCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = fileData->size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(fileData->data());

	VkResult result = vkCreateShaderModule(*device, &createInfo, nullptr, &_shaderModule);
	return result == VK_SUCCESS;
}
