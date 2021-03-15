#pragma once
#include <vector>

#include "ImageFlags.h"
#include "StageFlag.h"

using namespace std;

class IImage
{
public:	 
	IImage(ImageFormat format, ImageType type, ImageUsage usage, BufferStageFlag stage, int binding, int width, int height, int sampleCount, vector<unsigned char>& imageData);

	int Width() const;

	int Height() const;

	ImageType Type() const;

	ImageFormat Format() const;

	vector<unsigned char>* ImageData();

	ImageUsage Usage() const;

	BufferStageFlag Stage() const;

	int Binding() const;

	int SampleCount() const;
	
protected:

	const int _width;

	const int _height;

	int _binding;

	const int _sampleCount;
	
	vector<unsigned char> _imageData;

	ImageFormat _format;

	ImageType _type;

	ImageUsage _usage;

	BufferStageFlag _stage;
};
