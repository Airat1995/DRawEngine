#include "VulkanMeshData.h"


VulkanMeshData::VulkanMeshData(std::vector<IMesh*>& meshes) : _meshes(meshes)
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
	_meshes.push_back(mesh);
}

vector<IMesh*>& VulkanMeshData::Meshes()
{
	return _meshes;
}
