#pragma once
#include "IBuffer.h"

template <typename T>
class DynamicSizeBuffer :
	public IBuffer
{
public:

	DynamicSizeBuffer(BufferUsageFlag usage, BufferSharingMode sharingMode, T* data, BufferStageFlag stageFlag, int bindingId, int sizeOfObject, int itemsCount):
		_sizeOfObject(sizeOfObject), _itemsCount(itemsCount)
	{
		_usage = usage;
		_sharingMode = sharingMode;
		_data = data;
		_stageFlag = stageFlag;
		_bindingId = bindingId;
	}

	void SetItemsCount(int itemsCount)
	{
		_itemsCount = itemsCount;
	}

	const void* RawData() override
	{
		return _data;
	}

	int Size() override
	{
		return 20 + _sizeOfObject * _itemsCount;
	}

private:

	int _sizeOfObject;

	int _itemsCount;

protected:
	
	T* _data;
	
};