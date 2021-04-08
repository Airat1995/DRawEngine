#include "IMaterial.h"

IMaterial::IMaterial(map<ShaderType, IShader>& shaders, RenderQueue renderQueue) : _shaders(shaders), _renderQueue(renderQueue)
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

RenderQueue IMaterial::GetRenderQueue()
{
	return _renderQueue;
}
