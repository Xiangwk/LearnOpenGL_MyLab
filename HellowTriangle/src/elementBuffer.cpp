#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

#include <iostream>
#include <cmath>
#include <vector>

#include "Shader.h"

//window's width and height
GLuint WIDTH = 800;
GLuint HEIGHT = 600;

//the ids
enum VAO_IDs{ quad, NumVAOs };
enum Buffer_IDs{ quadBuffer, quadIndices, NumBuffers };
enum Attrib_IDs{ vPosition = 0 };
//the buffer used to store ids
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

//initialize GLFW window option
void initWindowOption();
//process input
void processInput(GLFWwindow *window);
//frame buffer size callback
void framebufferSizeCallback(GLFWwindow *window, int width, int height);

//this a vertex struct, we will encapsulate every vertex attribute in it
struct Vertex
{
	glm::vec3 position;    //position
};

int main()
{
	std::cout << "OpenGL 3.3 GO!" << std::endl;

	initWindowOption();

	//create a glfw window object
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "GLFW Window", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window!";
		glfwTerminate();
		std::abort();
	}
	glfwMakeContextCurrent(window);

	//initilize glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to init glad!";
		std::abort();
	}

	//create viewport
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	//quad's vertex data
	std::vector<Vertex> quadVertex{
		Vertex{ glm::vec3(-0.5f, -0.5f, 0.0f) },
		Vertex{ glm::vec3( 0.5f, -0.5f, 0.0f) },
		Vertex{ glm::vec3( 0.5f,  0.5f, 0.0f) },
		Vertex{ glm::vec3(-0.5f,  0.5f, 0.0f) }
	};
	//quad's indices
	std::vector<GLuint> quadElements{ 0, 1, 3, 1, 2, 3 };

	//create quad object
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[quad]);
	//transmit the data of quad's vertex and it's indices
	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[quadBuffer]);
	glBufferData(GL_ARRAY_BUFFER, quadVertex.size() * sizeof(Vertex), &quadVertex[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[quadIndices]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadElements.size() * sizeof(GLuint), &quadElements[0], GL_STATIC_DRAW);
	//set attribute pointer
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(vPosition);
	glBindVertexArray(0);

	//create shader
	Shader shader("quad.vert", "quad.frag");

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//set background color
	glClearColor(0.2, 0.3, 0.3, 1.0);

	//game loop
	while (!glfwWindowShouldClose(window))
	{
		//clear color buffer
		glClear(GL_COLOR_BUFFER_BIT);
		//process input
		processInput(window);
		//check events
		glfwPollEvents();

		//draw our quad with glDrawElements()
		glBindVertexArray(VAOs[quad]);
		shader.use();
		glDrawElements(GL_TRIANGLES, quadElements.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//swap framebuffer
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	std::cout << "Done!" << std::endl;

	return 0;
}

void initWindowOption()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//we have no need to use the compatibility profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void processInput(GLFWwindow *window)
{
	//if we press the key_esc, the window will be closed!
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}