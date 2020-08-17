#include "VulkanMeshData.h"

VulkanMeshData::VulkanMeshData(std::vector<IMesh*> meshes, vector<VulkanBuffer>& buffers, vector<VulkanImage>& images)
: _meshes(meshes), _buffers(buffers), _images(images), _needRebuild(true)
{	
	_bindingDescriptions = vector<VkVertexInputBindingDescription>();
	vector<VertexAttributeInfo> vertexAttributesBindings = meshes[0]->VertexInfo();
	vector<VertexBindingInfo> vertexBindings = meshes[0]->GetVertexBindingInfo();
	for (auto& vertexBinding : vertexBindings)
	{
		for (auto& attributeBinding : vertexAttributesBindings)
		{
			VkVertexInputAttributeDescription vertexAttribute = CreateAttributeDescription(vertexBinding.BindId, attributeBinding.Location,
				EnumFormatToVulkanFormat(attributeBinding.Format), attributeBinding.Offset);
			_attributeDescriptions.push_back(vertexAttribute);
		}

		VkVertexInputBindingDescription bindingDescriptions =
			CreateInputBindingDescription(vertexBinding.BindId,
				vertexBinding.Size, vertexBinding.InputRate);

		_bindingDescriptions.push_back(bindingDescriptions);
	}
}

std::vector<VkVertexInputBindingDescription> VulkanMeshData::BindingDescriptions()
{	
	return _bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> VulkanMeshData::AttributeDescriptions()
{
	return _attributeDescriptions;
}

void VulkanMeshData::AddMesh(IMesh* mesh)
{
	_needRebuild = true;
	_meshes.push_back(mesh);
	if(_bufferCreator != nullptr)
	{
		_bufferCreator->ReCreateBuffers(mesh);
	}
}

vector<IMesh*>& VulkanMeshData::Meshes()
{
	return _meshes;
}

vector<VulkanBuffer>& VulkanMeshData::Buffers()
{
	return _buffers;
}

vector<VulkanImage>& VulkanMeshData::Images()
{
	return _images;
}

void VulkanMeshData::SetBufferRecreateEventListener(IVulkanRenderMeshBufferCreator* bufferCreator)
{
	_bufferCreator = bufferCreator;
}

bool VulkanMeshData::SameShaders(IMesh* mesh)
{
	bool sameShaders = true;
	map<ShaderType, IShader> shaders = _meshes[0]->Shaders();
	map<ShaderType, IShader> meshSahders = mesh->Shaders();

	if (shaders.size() != meshSahders.size()) return false;
	bool containsSameShader;
	bool sameShaderData;
	for (auto && shader : shaders)
	{
		containsSameShader = meshSahders.count(shader.first) != 0;
		if(!containsSameShader)
		{
			sameShaders = false;
			break;
		}
		sameShaderData = meshSahders.at(shader.first).ShaderData() == shader.second.ShaderData();
		if(!sameShaderData)
		{
			sameShaders = false;
			break;
		}
	}

	return sameShaders;
}
