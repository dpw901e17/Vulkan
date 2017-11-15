#include "VulkanTest.h"
#include "HelloTriangleApplication.h"
#include <iostream>
#include <stdexcept>


// Called from main(). Runs this example.
int runVulkanTest() {
	HelloTriangleApplication app;

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