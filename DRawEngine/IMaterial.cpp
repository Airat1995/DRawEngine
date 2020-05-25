#include "IMaterial.h"

IMaterial::IMaterial(map<ShaderType, IShader>& shaders) : _shaders(shaders)
{
	_buffers = vector<IBuffer*>();
}

void IMaterial::AddBuffer(IBuffer* buffer)
{
	_buffers.push_back(buffer);
}

map<ShaderType, IShader>& IMaterial::MaterialShaders()
{
	return _shaders;
}

vector<IBuffer*>& IMaterial::Buffers()
{
	return _buffers;
}