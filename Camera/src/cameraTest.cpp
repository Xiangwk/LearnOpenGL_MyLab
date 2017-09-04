#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <stb_image.h>

#include <iostream>
#include <cmath>
#include <vector>

#include <Shader.h>

#include "Camera.h"

//window's width and height
GLuint WIDTH = 800;
GLuint HEIGHT = 600;

//the ids
enum VAO_IDs{ quad, NumVAOs };
enum Buffer_IDs{ quadBuffer, quadIndices, NumBuffers };
enum Attrib_IDs{ vPosition = 0, vColor = 1, vTexCoord = 2 };
enum Texture_IDs{ woodContainer, awesomeface, NumTextures };
//the buffer used to store ids
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint Textures[NumTextures];

glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
GLfloat pitch = 0.0f;
GLfloat yaw = -90.0f;

FreeCamera camera(cameraPos, worldUp, pitch, yaw);

GLfloat lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f;
GLboolean firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

//initialize GLFW window option
void initWindowOption();
//process input
void processInput(GLFWwindow *window);

void mouseCallback(GLFWwindow *window, double x, double y);

void scrollCallback(GLFWwindow *window, double x, double y);
//frame buffer size callback
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
//load texture image and bind to id
void loadTexture2D(const std::string &imageFile, GLuint id, bool flip);

//this a vertex struct, we will encapsulate every vertex attribute in it
struct Vertex
{
	glm::vec3 position;     //position
	glm::vec3 color;        //color
	glm::vec2 texCoord;     //texCoord
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
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//quad's vertex data
	std::vector<Vertex> quadVertex{
		//position                             //color                      //texCoord
		Vertex{ glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
		Vertex{ glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
		Vertex{ glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f) },
		Vertex{ glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) }
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
	//position
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(vPosition);
	//color
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(vColor);
	//texCoord
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(vTexCoord);

	glBindVertexArray(0);

	//create texture
	glGenTextures(NumTextures, Textures);
	loadTexture2D("../../../image/container.jpg", Textures[woodContainer], false);
	loadTexture2D("../../../image/awesomeface.png", Textures[awesomeface], true);

	//create shader
	Shader shader("quad.vert", "quad.frag");

	shader.use();
	shader.setUniformInt("Texture0", 0);
	shader.setUniformInt("Texture1", 1);
	//active texture unit
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Textures[woodContainer]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Textures[awesomeface]);

	//set background color
	glClearColor(0.2, 0.3, 0.3, 1.0);

	glm::mat4 trans = glm::mat4();
	trans = glm::rotate(trans, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	shader.setUniformMat4("model", trans);


	//game loop
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currTime = glfwGetTime();
		deltaTime = currTime - lastTime;
		lastTime = currTime;

		//clear color buffer
		glClear(GL_COLOR_BUFFER_BIT);
		//process input
		processInput(window);
		//check events
		glfwPollEvents();

		glm::mat4 cam = camera.getViewMatrix();
		glm::mat4 proj = glm::perspective(glm::radians(camera.zoom), GLfloat(WIDTH) / HEIGHT, 0.1f, 100.0f);
		shader.setUniformMat4("view", cam);
		shader.setUniformMat4("projection", proj);

		//draw our quad with glDrawElements() and use the textures
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

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, deltaTime);
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow *window, double x, double y)
{
	if (firstMouse)
	{
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	GLfloat xoffset = x - lastX;
	GLfloat yoffset = lastY - y;

	lastX = x;
	lastY = y;

	camera.processMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow *window, double x, double y)
{
	camera.processMouseScroll(y);
}

void loadTexture2D(const std::string &imageFile, GLuint id, bool flip)
{
	glBindTexture(GL_TEXTURE_2D, id);
	//set the wrapping/filtering option
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//load texture image
	GLint imageWidth, imageHeight, nrChannel;
	if (flip)
		stbi_set_flip_vertically_on_load(true);
	unsigned char *imageData = stbi_load(imageFile.c_str(), &imageWidth, &imageHeight, &nrChannel, 0);
	if (!imageData)
		std::cerr << "Failed to load texture\n";
	std::string format = imageFile.substr(imageFile.rfind('.') + 1);
	if (format == "jpg")
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	if (format == "png")
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(imageData);
}