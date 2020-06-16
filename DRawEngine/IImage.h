#pragma once
#include "ImageFlags.h"

class IImage
{
public:	 
	IImage(ImageFormat format, ImageType type, ImageUsage usage, int binding, int width, int height, unsigned char* imageData);

	int Width() const;

	int Height() const;

	ImageType Type() const;

	ImageFormat Format() const;

	unsigned char* ImageData() const;

	ImageUsage Usage() const;

	int Binding() const;
	
protected:

	int _width;

	int _height;

	int _binding;
	
	unsigned char* _imageData;

	ImageFormat _format;

	ImageType _type;

	ImageUsage _usage;
};
