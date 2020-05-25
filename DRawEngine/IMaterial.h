#pragma once

#include <string>
#include <map>

#include "IBuffer.h"
#include "IShader.h"

using namespace std;

class IMaterial
{
public:
	IMaterial(map<ShaderType, IShader>& shaders);

	void AddBuffer(IBuffer* buffer);

	map<ShaderType, IShader>& MaterialShaders();

	vector<IBuffer*>& Buffers();
private:
	
	map<ShaderType, IShader> _shaders;

	vector<IBuffer*> _buffers;
};