#pragma once
#include "ImageFlags.h"

class IImage
{
public:	 
	IImage(ImageFormat format, ImageType type, ImageUsage usage, int binding, int width, int height, int sampleCount, unsigned char* imageData);

	int Width() const;

	int Height() const;

	ImageType Type() const;

	ImageFormat Format() const;

	unsigned char* ImageData() const;

	ImageUsage Usage() const;

	int Binding() const;

	int SampleCount() const;
	
protected:

	const int _width;

	const int _height;

	int _binding;

	const int _sampleCount;
	
	unsigned char* _imageData;

	ImageFormat _format;

	ImageType _type;

	ImageUsage _usage;
};
