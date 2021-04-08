#pragma once

#include <string>
#include <map>

#include "IBuffer.h"
#include "IImage.h"
#include "IShader.h"
#include "RenderQueue.h"

using namespace std;

class IMaterial
{
public:
	IMaterial(map<ShaderType, IShader>& shaders, RenderQueue renderQueue);

	void AddBuffer(IBuffer* buffer);

	void AddImage(IImage* image);

	map<ShaderType, IShader>& MaterialShaders();

	vector<IBuffer*>& Buffers();

	vector<IImage*>& Images();

	RenderQueue GetRenderQueue();
private:
	
	map<ShaderType, IShader> _shaders;

	vector<IBuffer*> _buffers;

	vector<IImage*> _images;

	RenderQueue _renderQueue;
};