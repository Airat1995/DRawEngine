#pragma once
#include "IRender.h"
#include <string>

enum class WindowType;
using namespace std;

class IWindow
{
public:
	explicit IWindow(int width, int height, string name, WindowType windowType, IRender* render);

	virtual ~IWindow() = default;

	virtual void SetWidth(int width);

	virtual void SetHeight(int height);

	virtual void SetName(string name);

protected:
	int _width;
	
	int _height;
	
	string _name;

	IRender* _renderSystem;

private:
	const int MinWidth = 720;

	const int MinHeight = 1280;
};

enum class WindowType
{
	Fullscreen,
	Borderless,
	Windowed
};

