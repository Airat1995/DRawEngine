#include "SDLWindow.h"
#include <SDL2/SDL_vulkan.h>
#include "VulkanRender.h"
#include <iostream>

SDLWindow::SDLWindow(int width, int height, const string& name, WindowType windowType, IRender* render) : IWindow(width, height, name, windowType, render)
{
	Uint32 flag = WindowTypeToSDLWindowFlags(windowType);
	flag |= SDL_WINDOW_VULKAN | SDL_WINDOW_MOUSE_CAPTURE | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_INPUT_GRABBED;

	if(SDL_Init(SDL_INIT_EVERYTHING))
	{
		throw std::exception("Unable to initialize SDL!");
	}
	_window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flag);
	
    if(_window == nullptr)
    {
		throw std::exception("Unable to create SDL window!");
    }

	vector<const char*> extensions = GetExtensions();
	render->Init(&extensions);
	VulkanRender* vulkanInstance = dynamic_cast<VulkanRender*>(render);
	SDL_Vulkan_CreateSurface(_window, vulkanInstance->GetInstance(), vulkanInstance->GetSurface());
	vulkanInstance->InitSurface(width, height);
	vulkanInstance->CreateDepthBuffer();
	vulkanInstance->CreateBuffer();

}

vector<const char*> SDLWindow::GetExtensions() const
{
	// Get WSI extensions from SDL (we can add more if we like - we just can't remove these)
	unsigned extension_count;
	if (!SDL_Vulkan_GetInstanceExtensions(_window, &extension_count, nullptr)) 
		throw exception("Could not get the number of required instance extensions from SDL.");
	
	std::vector<const char*> extensions(extension_count);
	
	if (!SDL_Vulkan_GetInstanceExtensions(_window, &extension_count, extensions.data())) 
		throw exception("Could not get the names of required instance extensions from SDL.");
	return extensions;
}

SDLWindow::~SDLWindow()
{
	SDL_DestroyWindow(_window);
}

void SDLWindow::SetWidth(int width)
{
	IWindow::SetWidth(width);
	UpdateWindow();
}

void SDLWindow::SetHeight(int height)
{
	IWindow::SetHeight(height);
	UpdateWindow();
}

void SDLWindow::SetName(string name)
{
	IWindow::SetName(name);
	SDL_SetWindowTitle(_window, _name.c_str());
}

void SDLWindow::UpdateWindow() const
{
	SDL_SetWindowSize(_window, _width, _height);	
}

SDL_Window* SDLWindow::Window()
{
	return _window;
}

Uint32 SDLWindow::WindowTypeToSDLWindowFlags(WindowType windowType)
{
	switch (windowType)
	{
		case WindowType::Fullscreen:
			return SDL_WINDOW_FULLSCREEN;
		case WindowType::Borderless:
			return  SDL_WINDOW_FULLSCREEN_DESKTOP;
		default:
			return 0;
	}
}
