#pragma once
#include "IBuffer.h"

template <typename T>
class BaseBuffer :
	public IBuffer
{
public:

	BaseBuffer(BufferUsageFlag usage, BufferSharingMode sharingMode, T* data, BufferStageFlag stageFlag, int bindingId)
	{
		_usage = usage;
		_sharingMode = sharingMode;
		_data = data;
		_stageFlag = stageFlag;
		_bindingId = bindingId;
	}

	const void* RawData() override
	{
		return _data;
	}

	int Size() override
	{
		return sizeof(T);
	}

protected:
	T* _data;
};