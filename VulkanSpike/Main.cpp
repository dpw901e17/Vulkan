#include "VulkanTest.h"
#include "../../scene-window-system/TestConfiguration.h"
#include <string>
#include <sstream>

int main(int argc, char *argv[]) {

	std::stringstream arg;

	for (auto i = 0; i < argc; ++i) {
		arg << argv[i] << " "; 
	}

	TestConfiguration::SetTestConfiguration(arg.str().c_str());
	auto& conf = TestConfiguration::GetInstance();
	runVulkanTest();
	
}