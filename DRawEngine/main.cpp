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

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>

#include "Mesh.h"
#include "SDLWindow.h"
#include "SimpleVertex.h"
#include "VulkanRender.h"
#include "StageFlag.h"

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

Mesh<SimpleVertex1<ColoredVertexData>, ColoredVertexData> CreateMesh1()
{
	vector<SimpleVertex1<ColoredVertexData>> vertices = vector<SimpleVertex1<ColoredVertexData>>();

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "../viking_room.obj")) 
		throw std::runtime_error(warn + err);

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			ColoredVertexData* vertexData = new ColoredVertexData();

			vertexData->position = vec3(
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			);
			vertexData->color = vec3(1,1,1);
			vertices.emplace_back(SimpleVertex1(*vertexData));
		}
	}

	string vertexLoc = "../vert3.spv";
	string fragmentLoc = "../frag.spv";
	string name = "main";
	IShader vertexShader = IShader(vertexLoc, name, ShaderType::Vertex);
	IShader fragShader = IShader(fragmentLoc, name, ShaderType::Fragment);
	map<ShaderType, IShader> shaders = map<ShaderType, IShader>();
	shaders.insert(shaders.end(), std::pair<ShaderType, IShader>(ShaderType::Vertex, vertexShader));
	shaders.insert(shaders.end(), std::pair<ShaderType, IShader>(ShaderType::Fragment, fragShader));

	
	IMaterial simpleMaterial = IMaterial(shaders);
	Mesh<SimpleVertex1<ColoredVertexData>, ColoredVertexData> basicMesh = Mesh<SimpleVertex1<ColoredVertexData>, ColoredVertexData>(vertices, simpleMaterial);
	return basicMesh;
}

int main()
{
	IRender* vulkanRender = new VulkanRender();
	IWindow* window = new SDLWindow(1920, 1080, "some name", WindowType::Windowed, vulkanRender);

	auto mesh1 = CreateMesh1();

	float initialRotation = 90;
	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(initialRotation), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), 1920 / (float)1080, 0.1f, 10.0f);
	BaseBuffer<UniformBufferObject> uniformBuffer = BaseBuffer<UniformBufferObject>(BufferUsageFlag::UniformBuffer, BufferSharingMode::Exclusive, &ubo, BufferStageFlag::Fragment, 0);

	mesh1.Material().AddBuffer(&uniformBuffer);

	vulkanRender->AddMesh(&mesh1);
	
	bool stillRunning = true;
	SDL_Event event;
	
	while (stillRunning)
	{
		initialRotation++;
		ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(initialRotation), glm::vec3(0.0f, 0.0f, 1.0f));
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
			stillRunning = false;
		vulkanRender->DrawFrame();
	}

	return 0;
}
