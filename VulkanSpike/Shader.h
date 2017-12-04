#pragma once
#include <vulkan/vulkan.hpp>
#include "Utility.h"

class Shader
{
public:
	Shader(VkDevice device, std::string file_path, VkShaderStageFlagBits stage);
	~Shader();

	VkPipelineShaderStageCreateInfo m_Info;
private:
	VkDevice m_Device;
	VkShaderModule m_Module;

};
