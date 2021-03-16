#include "HeightMap.h"

HeightMap::HeightMap(ImageFormat format, ImageType type, ImageUsage usage, BufferStageFlag stage, int binding, int width, int height, int sampleCount, vector<unsigned char>& imageData, int patchSize, float uvScale)
	:IImage(format, type, usage, stage, binding, width, height, sampleCount, imageData), _patchSize(patchSize), _uvScale(uvScale)
{
	_scale = width / patchSize;
	heightdata = new uint16_t[_imageData.size()];
	memcpy(heightdata, _imageData.data(), _imageData.size());
}

float HeightMap::GetHeight(int xPos, int yPos)
{
	ivec2 rPos = ivec2(xPos, yPos) * ivec2(_scale);
	rPos.x = glm::max(0, glm::min(rPos.x, (int)_width - 1));
	rPos.y = glm::max(0, glm::min(rPos.y, (int)_width - 1));
	rPos /= glm::ivec2(_scale);
	return *(heightdata + (rPos.x + rPos.y * _width) * (int) _scale) / 65535.0f;
}
