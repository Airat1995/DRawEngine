/*
 * Vulkan Windowed Program
 *
 * Copyright (C) 2016, 2018 Valve Corporation
 * Copyright (C) 2016, 2018 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
Vulkan Windowed Project Template
Create and destroy a Vulkan surface on an SDL window.
*/

// Enable the WSI extensions
#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>

#include "IMesh.h"
#include "SDLWindow.h"
#include "SimpleVertex.h"
#include "VulkanRender.h"

IMesh<SimpleVertex<AlphaColoredVertexData>, AlphaColoredVertexData> CreateMesh()
{
	vector<SimpleVertex<AlphaColoredVertexData>> vertices = vector<SimpleVertex<AlphaColoredVertexData>>();
	AlphaColoredVertexData firstVertex = AlphaColoredVertexData();
	firstVertex.position = vec2(-1, -1);
	firstVertex.color = vec3(1, 0, 0);
	firstVertex.alpha = 1;

	AlphaColoredVertexData firstVertex2 = AlphaColoredVertexData();
	firstVertex2.position = vec2(1, 1);
	firstVertex2.color = vec3(0, 1, 0);
	firstVertex2.alpha = 0.5;

	AlphaColoredVertexData firstVertex3 = AlphaColoredVertexData();
	firstVertex3.position = vec2(-1, 1);
	firstVertex3.color = vec3(0, 0, 1);
	firstVertex3.alpha = 0.0;

	SimpleVertex vertex1 = SimpleVertex(firstVertex);
	SimpleVertex vertex2 = SimpleVertex(firstVertex2);
	SimpleVertex vertex3 = SimpleVertex(firstVertex3);
	vertex1.FillVertexInfo();
	vertices.push_back(vertex1);
	vertices.push_back(vertex2);
	vertices.push_back(vertex3);
	string vertexLoc = "../vert.spv";
	string fragmentLoc = "../frag.spv";
	string name = "main";
	IShader vertexShader = IShader(vertexLoc, name, ShaderType::Vertex);
	IShader fragShader = IShader(fragmentLoc, name, ShaderType::Fragment);
	vector<IShader> shaders = vector<IShader>();
	shaders.push_back(vertexShader);
	shaders.push_back(fragShader);

	IMesh<SimpleVertex<AlphaColoredVertexData>, AlphaColoredVertexData> basicMesh = IMesh<SimpleVertex<AlphaColoredVertexData>, AlphaColoredVertexData>(vertices, shaders);
	return basicMesh;
}

IMesh<SimpleVertex1<ColoredVertexData>, ColoredVertexData> CreateMesh1()
{
	vector<SimpleVertex1<ColoredVertexData>> vertices = vector<SimpleVertex1<ColoredVertexData>>();
	ColoredVertexData firstVertex = ColoredVertexData();
	firstVertex.position = vec2(1, 1);
	firstVertex.color = vec3(1, 0, 0);

	ColoredVertexData firstVertex2 = ColoredVertexData();
	firstVertex2.position = vec2(-1, -1);
	firstVertex2.color = vec3(0, 1, 0);

	ColoredVertexData firstVertex3 = ColoredVertexData();
	firstVertex3.position = vec2(1, -1);
	firstVertex3.color = vec3(0, 0, 1);

	SimpleVertex1 vertex1 = SimpleVertex1(firstVertex);
	SimpleVertex1 vertex2 = SimpleVertex1(firstVertex2);
	SimpleVertex1 vertex3 = SimpleVertex1(firstVertex3);
	vertex1.FillVertexInfo();
	vertices.push_back(vertex1);
	vertices.push_back(vertex2);
	vertices.push_back(vertex3);
	string vertexLoc = "../vert1.spv";
	string fragmentLoc = "../frag.spv";
	string name = "main";
	IShader vertexShader = IShader(vertexLoc, name, ShaderType::Vertex);
	IShader fragShader = IShader(fragmentLoc, name, ShaderType::Fragment);
	vector<IShader> shaders = vector<IShader>();
	shaders.push_back(vertexShader);
	shaders.push_back(fragShader);

	IMesh<SimpleVertex1<ColoredVertexData>, ColoredVertexData> basicMesh = IMesh<SimpleVertex1<ColoredVertexData>, ColoredVertexData>(vertices, shaders);
	return basicMesh;
}

int main()
{
	IRender* vulkanRender = new VulkanRender();
	IWindow* window = new SDLWindow(1920, 1080, "some name", WindowType::Windowed, vulkanRender);

	IMesh<SimpleVertex<AlphaColoredVertexData>, AlphaColoredVertexData> basicMesh = CreateMesh();
	auto mesh1 = CreateMesh1();
	vulkanRender->AddMesh(&basicMesh);
	vulkanRender->AddMesh(&mesh1);
	
    bool stillRunning = true;
	SDL_Event event;
    while(stillRunning) 
	{		
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
			stillRunning = false;
		vulkanRender->DrawFrame();	
    }
//
//    // Clean up.
//    vkDestroySurfaceKHR(instance, surface, NULL);
//    SDL_DestroyWindow(window);
//    SDL_Quit();
//    vkDestroyInstance(instance, NULL);

    return 0;
}
