#pragma once

#include "StageFlag.h"

enum class BufferSharingMode
{
	Exclusive = 0,
	Concurrent = 1,
};

enum class BufferUsageFlag
{
	TransferSrc = 0x00000001,
	TransferDst = 0x00000002,
	UniformTexel = 0x00000004,
	StorageTexel = 0x00000008,
	UniformBuffer = 0x00000010,
	StorageBuffer = 0x00000020,
	IndexBuffer = 0x00000040,
	VertexBuffer = 0x00000080,
	IndirectBuffer = 0x00000100,
	TransformFeedbackBuffer = 0x00000800,
	TransformFeedbackCounter = 0x00001000,
	ConditionalRendering = 0x00000200,
	RayTracingNV = 0x00000400,
	ShaderDeviceAddress = 0x00020000
};

class IBuffer
{
public:

	virtual const void* RawData() = 0;

	virtual int Size() = 0;

	BufferUsageFlag Usage()
	{
		return _usage;
	}

	BufferSharingMode SharingMode()
	{
		return _sharingMode;
	}

	virtual BufferStageFlag StageFlag()
	{
		return _stageFlag;
	}

	int BindingId()
	{
		return _bindingId;
	}

protected:
	BufferUsageFlag _usage;

	BufferSharingMode _sharingMode;

	BufferStageFlag _stageFlag;

	int _bindingId;
};