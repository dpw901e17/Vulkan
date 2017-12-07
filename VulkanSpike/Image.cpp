#include "Image.h"

Image::Image(const Device& device, const vk::ImageCreateInfo& imageCreateInfo, const vk::MemoryPropertyFlags& properties, const vk::ImageAspectFlagBits& aspect_flags): m_Format(imageCreateInfo.format), m_Device(device)
{
	m_Image = device->createImage(imageCreateInfo);

	//copy to buffer
	VkMemoryRequirements memRequirements = device->getImageMemoryRequirements(m_Image);

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(static_cast<vk::PhysicalDevice>(device), memRequirements.memoryTypeBits, properties);


	m_Memory = device->allocateMemory(allocInfo);

	device->bindImageMemory(m_Image, m_Memory, 0);

	vk::ImageSubresourceRange subresourceRange;
	subresourceRange.setAspectMask(aspect_flags)
	                .setLevelCount(1)
	                .setLayerCount(1);

	vk::ImageViewCreateInfo view_info;
	view_info.setImage(m_Image)
	         .setViewType(vk::ImageViewType::e2D)
	         .setFormat(m_Format)
	         .setSubresourceRange(subresourceRange);

	m_ImageView = device->createImageView(view_info);
}

Image::~Image()
{
	m_Device->destroyImageView(m_ImageView);
	m_Device->destroyImage(m_Image);
	m_Device->freeMemory(m_Memory);
}

void Image::changeLayout(const vk::ImageLayout& newLayout, const CommandPool& command_Pool)
{
	auto commandBuffer = command_Pool.beginSingleTimeCommands();

	vk::ImageAspectFlags aspect_mask = vk::ImageAspectFlagBits::eColor;

	// Special case for depth buffer image
	if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		aspect_mask = vk::ImageAspectFlagBits::eDepth;
		if (m_Format == vk::Format::eD32SfloatS8Uint || m_Format == vk::Format::eD24UnormS8Uint)
		{
			aspect_mask |= vk::ImageAspectFlagBits::eStencil;
		}
	}

	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;
	vk::AccessFlags source_access_mask;
	vk::AccessFlags destination_access_mask;

	if (m_ImageLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		source_access_mask = vk::AccessFlags();
		destination_access_mask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (m_ImageLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		source_access_mask = vk::AccessFlagBits::eTransferWrite;
		destination_access_mask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else if (m_ImageLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		source_access_mask = vk::AccessFlags();
		destination_access_mask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}

	auto image_memory_barrier = vk::ImageMemoryBarrier(
		source_access_mask,
		destination_access_mask,
		m_ImageLayout,
		newLayout,
		VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED,
		m_Image,
		{aspect_mask, 0, 1, 0, 1});

	commandBuffer.pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags(), {}, {}, {image_memory_barrier});

	command_Pool.endSingleTimeCommands(commandBuffer);

	m_ImageLayout = newLayout;
}
