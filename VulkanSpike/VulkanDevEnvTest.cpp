#if false //some definitions clash if this code is compiled

// #include "VulkanTest.h" //<-- uncomment to run this example from main

#ifndef GLFW_INCLUDE_VULKAN
	#define GLFW_INCLUDE_VULKAN
#endif // !GLFW_INCLUDE_VULKAN

#include <glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <linmath.h>

#include <iostream>

int runVulkanTest()
{
	int windowWidth = 800;
	int windowHeight = 600;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Vulkan Spike Test", nullptr, nullptr);

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	
	std::cout << extensionCount << " extensions supported!" << std::endl;	

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
	
	glfwDestroyWindow(window);

	glfwTerminate();

	return EXIT_SUCCESS;
}

#endif