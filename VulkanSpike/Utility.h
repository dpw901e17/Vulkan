#pragma once
#include <vector>
#include <fstream>

static std::vector<char> readFile(const std::string& filename) {

	//ate: read from the end of file
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file " + filename);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	return buffer;
}

static uint32_t findMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

	for (auto i = 0; i < memProperties.memoryTypeCount; i++) {
		if (typeFilter & 1 << i && memProperties.memoryTypes[i].propertyFlags & properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}