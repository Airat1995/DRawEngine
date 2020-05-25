#pragma once

//FLAGS SHOULDN`T BE CALCULATED BY SWITCH
enum class BufferStageFlag : unsigned int
{
	Vertex = 1 << 0,
	TesselationControl = 1 << 1,
	TesselationEvaluation = 1 << 2,
	Geometry = 1 << 3,
	Fragment = 1 << 4
};

inline constexpr BufferStageFlag operator|(BufferStageFlag a, BufferStageFlag b) 
{
	return a = static_cast<BufferStageFlag> (static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline constexpr BufferStageFlag operator&(BufferStageFlag a, BufferStageFlag b) 
{
	return a = static_cast<BufferStageFlag> (static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

static inline constexpr bool HasFlag(BufferStageFlag a, BufferStageFlag b)
{
	return static_cast<BufferStageFlag> (a & b) == b;	
}