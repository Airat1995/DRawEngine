#pragma once
#include <stdlib.h>

#include "IImage.h"
#include "glm/glm.hpp"

using namespace glm;

class HeightMap :
    public IImage
{
public:
	HeightMap(ImageFormat format, ImageType type, ImageUsage usage, BufferStageFlag stage, int binding, int width, int height, int sampleCount, vector<unsigned char>& imageData, int patchSize = 64, float uvScale = 1.0f);

	float GetHeight(int xPos, int yPos);

protected:
	int _patchSize;

	float _scale;

	float _uvScale;

	uint16_t* heightdata;
};

