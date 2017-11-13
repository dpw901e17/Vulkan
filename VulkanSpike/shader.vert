#version 450
#extension GL_ARB_separate_shader_objects : enable //<-- needs to be there for Vulkan to work

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

//output to be sent through the entire rest of pipeline.
out gl_PerVertex {
	vec4 gl_Position;
};

void main() {
	gl_Position = vec4(inPosition, 0.0, 1.0);
	fragColor = inColor;
}