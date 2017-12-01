#pragma once
#include <vulkan/vulkan.hpp>
#include "Utility.h"

class Image
{
public:
    Image(VkDevice device, VkPhysicalDevice physical_device, VkImageCreateInfo imageCreateInfo, VkMemoryPropertyFlags properties)
	{
		if (vkCreateImage(device, &imageCreateInfo, nullptr, &m_Image) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image!");
		}

		//copy to buffer
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, m_Image, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(physical_device, memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &m_Memory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, m_Image, m_Memory, 0);
	}
private:
	VkImage m_Image;
	VkDeviceMemory m_Memory;
};
