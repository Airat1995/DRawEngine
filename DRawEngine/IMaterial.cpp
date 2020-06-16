#include "IMaterial.h"

IMaterial::IMaterial(map<ShaderType, IShader>& shaders) : _shaders(shaders)
{
	_buffers = vector<IBuffer*>();
}

void IMaterial::AddBuffer(IBuffer* buffer)
{
	_buffers.push_back(buffer);
}

void IMaterial::AddImage(IImage* image)
{
	_images.push_back(image);
}

map<ShaderType, IShader>& IMaterial::MaterialShaders()
{
	return _shaders;
}

vector<IBuffer*>& IMaterial::Buffers()
{
	return _buffers;
}

vector<IImage*>& IMaterial::Images()
{
	return _images;
}
