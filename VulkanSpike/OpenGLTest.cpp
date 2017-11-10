#include <glad/glad.h> // Makes newer OpenGL functions accessible
#include <glfw3.h> // Wrapper itself
#include <linmath.h> // Linear math 
#include <stdlib.h> 
#include <stdio.h>
#include "OpenGLTest.h"

#define _TRIPPYTRIAG	true

// Struct for vertexes
static const struct
{
	float x, y; // Positions
	float r, g, b; // Colour
} vertices[3] =
{
	{ -0.6f, -0.4f, 1.f, 0.f, 0.f },
	{ 0.6f, -0.4f, 0.f, 1.f, 0.f },
	{ 0.f,  0.6f, 0.f, 0.f, 1.f }
};
// Code for vertex shader. Places vertexes on screen using matrixes.
// Triangle is drawn from these vertexes at a later time.
static const char* vertex_shader_text =
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";
// Code for fragment shader.
// Fragments focus on individual pixels, if they're shaded and how.
// Are produced by the rasterizer, which displays objects in a pixel by pixel grid format.
// Here we allow the color of the fragments to blend together.
static const char* fragment_shader_text =
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";
// Called by GLFW in case of error
static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}
// Called by GLFW in case of a key press/release
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}
int runOpenGL()
{
	GLFWwindow* window;
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;

	glfwSetErrorCallback(error_callback); // Pass function pointer to glfw for error reporting

										  // Check that glfw initializes correctly
	if (!glfwInit())
		exit(EXIT_FAILURE);
	// When there is room for interpretation in GLFW behavior hints can be used. 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	// Make a combined window and openGL context
	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetKeyCallback(window, key_callback);

	// Set current OpenGL context. A context is like an instance of OpenGL with all related data.
	glfwMakeContextCurrent(window);

	// Initialize glad now that the context is set. Allows for calls to newer OpenGL functions.
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	// Set to one to swap between front and back buffer once per frame.
	// Default would be 0, swapping buffers as fast as possible creating tearing.
	// How many frames we wait until swap is known as vsync.
	glfwSwapInterval(1);
	// Set up buffer and load initial vertexes into it
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // Binds GL_ARRAY_BUFFER to the buffer object. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Vertex insertion

																			   // Compiling vertex shader
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);
	// Compiling fragment shader
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);
	// Linking shaders together into a program
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	// Setting up location of arguments.
	mvp_location = glGetUniformLocation(program, "MVP");
	vpos_location = glGetAttribLocation(program, "vPos");
	vcol_location = glGetAttribLocation(program, "vCol");

	// Setting up vpos attribute in shader. Position of Vertex.
	// Attributes are per vertex data
	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
		sizeof(float) * 5, (void*)0);
	// Setting up vcol attribute in shader. Color of Vertex 
	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
		sizeof(float) * 5, (void*)(sizeof(float) * 2));

	//Render loop that runs until window is closed.
	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		mat4x4 m, p, mvp;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT); // Clearing of the buffer
									  // Creating a matrix to be used in rendering the vertexes
		mat4x4_identity(m);
		mat4x4_rotate_Z(m, m, (float)glfwGetTime()); // Rotates identity matrix by a degree depending on time elapsed.
#if _TRIPPYTRIAG
		mat4x4_rotate_X(m, m, (float)glfwGetTime() / 2); //<-- kaboom!
		mat4x4_rotate_Y(m, m, (float)glfwGetTime() * 2);
#endif
		mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f); // Projects the 3d figure onto a 2d plane. Defining clipping areas.
		mat4x4_mul(mvp, p, m); // Projection onto plane.

							   // Drawing
		glUseProgram(program);
		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp); // Setting the matrix to be a uniform. This is global data for each vertex to be rendered.
		glDrawArrays(GL_TRIANGLES, 0, 3); // Specifies a geometric primitive to be rendered from the array of vertexes given. 
		glfwSwapBuffers(window); // swap to back buffer
		glfwPollEvents(); // Processes polled events such as key presses.
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}