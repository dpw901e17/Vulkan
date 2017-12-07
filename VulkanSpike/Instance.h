#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

class Instance
{
public:
	Instance();
	Instance(const Instance&) = delete;
	~Instance();

	const vk::Instance* operator ->() const{ return &m_Instance; }
	explicit operator vk::Instance() const { return m_Instance; }
	explicit operator VkInstance() const { return static_cast<VkInstance>(m_Instance); }
private:
	static const std::vector<const char*> s_RequiredExtensions;
	vk::Instance m_Instance;
#ifndef NDEBUG
	static const std::vector<const char*> s_ValidationLayers;
	static VkBool32 __stdcall debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userdata);
	bool checkSingleValidationLayerSupport(const char* layer_name) const;
	bool checkValidationLayerSupport() const;
	VkDebugReportCallbackEXT m_Callback;
#endif
};
