#include "VulkanTest.h"
#include "HelloTriangleApplication.h"
#include <iostream>
#include <stdexcept>
#include "../scene-window-system/Scene.h"


const int WIDTH = 800;
const int HEIGHT = 600;


int runVulkanTest() {

	auto& testConfig = TestConfiguration::GetInstance();

	auto cubeCountPerDim = testConfig.cubeDimension;
	auto paddingFactor = testConfig.cubePadding;

	Window win = Window(GetModuleHandle(nullptr), "VulkanTest", "Vulkan Test", WIDTH, HEIGHT);

	Camera camera = Camera::Default();
	auto heightFOV = camera.FieldOfView() / win.aspectRatio();
	auto base = (cubeCountPerDim + (cubeCountPerDim - 1) * paddingFactor) / 2.0f;
	auto camDistance = base / std::tan(heightFOV / 2);
	float z = camDistance + base + camera.Near();

	camera.SetPosition({ 0.0f, 0.0f, z, 1.0f });
	camera.SetFar(z + base + camera.Near());
	auto scene = Scene(camera, cubeCountPerDim, paddingFactor);

	HelloTriangleApplication app(scene, win);

	try {
		app.run();
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	system("pause");
	exit(EXIT_SUCCESS);
}