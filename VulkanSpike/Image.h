#pragma once
#include <vulkan/vulkan.hpp>
#include "Utility.h"

class Image
{
public:
	Image(VkDevice device, VkPhysicalDevice physical_device, VkImageCreateInfo imageCreateInfo, VkMemoryPropertyFlags properties, VkImageAspectFlagBits aspect_flags)
		: m_Device(device), m_Format(imageCreateInfo.format)
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

		VkImageViewCreateInfo view_info = {};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = m_Image;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = m_Format;
		view_info.subresourceRange.aspectMask = aspect_flags;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &view_info, nullptr, &m_ImageView) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image view!");
		}
	}

	~Image()
    {
		vkDestroyImageView(m_Device, m_ImageView, nullptr);
		vkDestroyImage(m_Device, m_Image, nullptr);
		vkFreeMemory(m_Device, m_Memory, nullptr);
    }


	VkImage m_Image;
	VkDeviceMemory m_Memory;
	VkImageView m_ImageView;
	VkFormat m_Format;
private:
	VkDevice m_Device;
};
