#include "IWindow.h"
#include <utility>

IWindow::IWindow(int width, int height, string name, WindowType windowType, IRender* render)
{
	_width = width;
	_height = height;
	_name = move(name);
	_renderSystem = render;
}

void IWindow::SetWidth(int width)
{
	if(width == _width)
		return;
	if (width < MinWidth)
		width = MinWidth;
	_width = width;
}

void IWindow::SetHeight(int height)
{
	if (height == _height)
		return;
	if (height < MinHeight)
		height = MinHeight;
	_height = height;

}

void IWindow::SetName(string name)
{
	_name = move(name);
}
