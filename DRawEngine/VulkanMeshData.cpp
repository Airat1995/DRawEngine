#include "VulkanMeshData.h"

VulkanMeshData::VulkanMeshData(IMesh* mesh, vector<VulkanBuffer>& buffers, vector<VulkanImage>& images, vector<VulkanBuffer>& perObjectBuffers)
	: _buffers(buffers), _images(images), _needRebuild(true)
{
	_perObjectBuffers = map<IMesh*, vector<VulkanBuffer>>();
	_meshes = vector<IMesh*>();
	_meshes.push_back(mesh);

	_perObjectBuffers.insert({ mesh, perObjectBuffers });
	
	_bindingDescriptions = vector<VkVertexInputBindingDescription>();
	vector<VertexAttributeInfo> vertexAttributesBindings = mesh->VertexInfo();
	vector<VertexBindingInfo> vertexBindings = mesh->GetVertexBindingInfo();
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

void VulkanMeshData::AddMesh(IMesh* mesh, vector<VulkanBuffer>& vulkanBuffers)
{
	_needRebuild = true;
	_meshes.push_back(mesh);
	if(_bufferCreator != nullptr)
	{
		_perObjectBuffers.insert({ mesh, vulkanBuffers });
		vector<VulkanBuffer> buffers = PerObjectBuffersInfo(mesh);
		_bufferCreator->ReCreateBuffers(mesh, buffers);
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

vector<VulkanBuffer>& VulkanMeshData::PerObjectBuffersInfo()
{
	return _perObjectBuffers.at(_meshes.at(0));
}

vector<VulkanBuffer>& VulkanMeshData::PerObjectBuffersInfo(IMesh* mesh)
{
	return _perObjectBuffers.at(mesh);
}

void VulkanMeshData::SetBufferRecreateEventListener(IVulkanRenderMeshBufferCreator* bufferCreator)
{
	_bufferCreator = bufferCreator;
}

bool VulkanMeshData::ShouldCombine(IMesh* mesh)
{
	bool sameShaders = true;
	map<ShaderType, IShader> shaders = _meshes[0]->Shaders();
	map<ShaderType, IShader> meshSahders = mesh->Shaders();

	if (shaders.size() != meshSahders.size()) return false;
	bool containsSameShader;
	bool sameShaderData;
	bool isBothSameStaticType = _meshes[0]->IsStatic() && mesh->IsStatic();
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

	return sameShaders && isBothSameStaticType;
}
