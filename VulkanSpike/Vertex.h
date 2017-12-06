#pragma once
#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec3.hpp>
#include <vulkan/vulkan.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texCoord;

	static vk::VertexInputBindingDescription getBindingDescription()
	{
		vk::VertexInputBindingDescription binding_description;
		binding_description.binding = 0;
		binding_description.stride = sizeof(Vertex);
		binding_description.inputRate = vk::VertexInputRate::eVertex; 

		return binding_description;
	}

	static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions()
	{
		std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = vk::Format::eR32G32Sfloat;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}
};