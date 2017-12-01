#include "Buffer.h"
#include "Utility.h"

Buffer::Buffer(VkPhysicalDevice physicalDevice, VkDevice device, VkBufferCreateInfo buffer_info, VkMemoryPropertyFlags properties)
	: m_Device(device), m_Size(buffer_info.size)
{
	if (vkCreateBuffer(device, &buffer_info, nullptr, &m_Buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, m_Buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &m_Memory))
	{
		throw std::runtime_error("Failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(device, m_Buffer, m_Memory, 0);
}

Buffer::~Buffer()
{
	vkDestroyBuffer(m_Device, m_Buffer, nullptr);
	vkFreeMemory(m_Device, m_Memory, nullptr);
}

VkResult Buffer::map()
{
	return vkMapMemory(m_Device, m_Memory, 0, VK_WHOLE_SIZE, 0, &m_Mapped);
}

void Buffer::unmap()
{
	vkUnmapMemory(m_Device, m_Memory);
	m_Mapped = nullptr;
}
