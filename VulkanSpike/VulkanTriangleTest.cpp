#include "VulkanTest.h"
#include "HelloTriangleApplication.h"
#include <iostream>
#include <stdexcept>
#include "../scene-window-system/Scene.h"


// Called from main(). Runs this example.
int runVulkanTest() {
	Scene scene(Camera::Default(), { RenderObject(0.0f, 0.0f, 0.0f) });
	HelloTriangleApplication app(scene);

	try {
		app.run();
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}