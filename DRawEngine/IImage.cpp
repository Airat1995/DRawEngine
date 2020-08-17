#include "IImage.h"

IImage::IImage(ImageFormat format, ImageType type, ImageUsage usage, int binding, int width, int height, int sampleCount, unsigned char* imageData)
	: _width(width), _height(height), _binding(binding), _sampleCount(sampleCount), _imageData(imageData), _format(format), _type(type), _usage(usage)
{
}

int IImage::Width() const
{
	return _width;
}

int IImage::Height() const
{
	return _height;
}

ImageType IImage::Type() const
{
	return _type;
}

ImageFormat IImage::Format() const
{
	return _format;
}

unsigned char* IImage::ImageData() const
{
	return _imageData;
}

ImageUsage IImage::Usage() const
{
	return _usage;
}

int IImage::Binding() const
{
	return _binding;
}

int IImage::SampleCount() const
{
	return _sampleCount;
}
