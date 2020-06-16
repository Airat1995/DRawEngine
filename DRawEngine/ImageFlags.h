#pragma once

enum class ImageFormat
{
	R,
	RG,
	RGB,
	RGBA,
};

enum class ImageType
{
	_1D,
	_2D,
	_3D,
	Cube,
	Array1D,
	Array2D,
	CubeArray
};

enum class ImageUsage
{
	TransferSrc = 1 << 0,
	TransferDst = 1 << 1,
	Sample = 1 << 2,
	Storage = 1 << 3,
	DepthStencil = 1 << 4,
	TransientAtc = 1 << 5,
	InputAtc = 1 << 6,
	ShadingRate = 1 << 7,
	FragmentDensity = 1 << 8
};

inline constexpr ImageUsage operator|(ImageUsage a, ImageUsage b)
{
	return a = static_cast<ImageUsage> (static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline constexpr ImageUsage operator&(ImageUsage a, ImageUsage b)
{
	return a = static_cast<ImageUsage> (static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

static inline constexpr bool HasFlag(ImageUsage a, ImageUsage b)
{
	return static_cast<ImageUsage> (a & b) == b;
}