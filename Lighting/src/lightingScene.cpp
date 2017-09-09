#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <stb_image.h>

#include <Shader.h>
#include <Camera.h>
#include "Light.h"
#include "VertexDataLoader.h"

#include <iostream>
#include <cmath>

//window's width and height
GLuint WIDTH = 800;
GLuint HEIGHT = 600;

enum VAO_IDs{ box, lamp, NumVAOs };
enum Buffer_IDs{ boxBuffer, lampBuffer, NumBuffers };
enum Texture_IDs{ wood, iron, NumTextures };
enum Uniform_IDs{ lights, vpMatirx, NumUniforms };
enum Attrib_Ids{ vPostion, vNormal, vTexCoord };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint Textures[NumTextures];
GLuint Uniforms[NumUniforms];

glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
FreeCamera camera(cameraPos);

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

bool firstMouse = true;
//the cursor's position in last frame
GLfloat lastX = GLfloat(WIDTH) / 2;
GLfloat lastY = GLfloat(HEIGHT) / 2;

const GLint PointLightNum = 4;
const GLint SpotLightNum = 1;

const glm::vec3 boxPositions[] = 
{
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(2.0f, 5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f, 3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f, 2.0f, -2.5f),
	glm::vec3(1.5f, 0.2f, -1.5f),
	glm::vec3(-1.3f, 1.0f, -1.5f)
};

const glm::vec3 pointLightPositions[PointLightNum] = 
{
	glm::vec3(0.7f, 0.2f, 2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f, 2.0f, -12.0f),
	glm::vec3(0.0f, 0.0f, -3.0f)
};

//initialize GLFW window option
void initWindowOption();
//process input
void processInput(GLFWwindow *window);
//frame buffer size callback
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void mouseCallback(GLFWwindow *window, double x, double y);
void scrollCallback(GLFWwindow *window, double x, double y);
void loadTexture2D(const std::string &imageFile, GLuint &id, bool flip);

int main()
{
	std::cout << "OpenGL 3.3 GO! Let's make some fun!" << std::endl;

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
	//set background color
	glClearColor(0.0, 0.0, 0.0, 1.0);
	//set callback
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	VertDataLoader boxLoader;
	boxLoader.loadData("boxVertexData.txt");
	std::vector<Vertex> boxData = boxLoader.data;

	//create box
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[box]);
	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[boxBuffer]);
	glBufferData(GL_ARRAY_BUFFER, boxData.size() * sizeof(Vertex), &boxData[0], GL_STATIC_DRAW);
	glVertexAttribPointer(vPostion, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0));
	glEnableVertexAttribArray(vPostion);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(vTexCoord);
	glBindVertexArray(0);

	//create lamp
	glBindVertexArray(VAOs[lamp]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[lampBuffer]);
	glBufferData(GL_ARRAY_BUFFER, boxData.size() * sizeof(Vertex), &boxData[0], GL_STATIC_DRAW);
	glVertexAttribPointer(vPostion, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0));
	glEnableVertexAttribArray(vPostion);
	glBindVertexArray(0);

	Shader boxShader("box.vert", "box.frag");
	Shader lampShader("lamp.vert", "lamp.frag");

	
	glGenBuffers(NumUniforms, Uniforms);
	//create view projection matrix ubo
	glBindBuffer(GL_UNIFORM_BUFFER, Uniforms[vpMatirx]);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, Uniforms[vpMatirx]);
	//create lights ubo
	glBindBuffer(GL_UNIFORM_BUFFER, Uniforms[lights]);
	glBufferData(GL_UNIFORM_BUFFER, 64 + PointLightNum * 80 + SpotLightNum * 112, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, Uniforms[lights]);

	//directional light
	DirLight dLight
	{
		glm::vec3(-0.2f, -1.0f, -0.3f),
		glm::vec3(0.02f, 0.02f, 0.02f),
		glm::vec3(0.3f, 0.3f, 0.3f),
		glm::vec3(0.5f, 0.5f, 0.5f)
	};
	//point lights
	std::vector<PointLight> pLights;
	for (size_t i = 0; i < PointLightNum; ++i)
	{
		PointLight pLight
		{
			pointLightPositions[i],
			glm::vec3(0.05f, 0.05f, 0.05f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			{ 1.0f, 0.09f, 0.032f }
		};
		pLights.push_back(pLight);
	}
	//spot lights
	std::vector<SpotLight> sLights;
	for (size_t i = 0; i < SpotLightNum; ++i)
	{
		SpotLight torch
		{
			camera.position,
			camera.front,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			{ 1.0f, 0.09f, 0.032f },
			{ glm::cos(glm::radians(8.0f)), glm::cos(glm::radians(15.0f)) }
		};
		sLights.push_back(torch);
	}
	//set the lights uniform
	dLight.setUniform(Uniforms[lights], 0);
	for (size_t i = 0; i < PointLightNum; ++i)
	{
		GLuint baseOffset = 64 + i * 80;
		pLights[i].setUniform(Uniforms[lights], baseOffset);
	}

	//create diffuse texture and specular texture
	glGenTextures(NumTextures, Textures);
	loadTexture2D("../../../image/container2.png", Textures[wood], false);
	loadTexture2D("../../../image/container2_specular.png", Textures[iron], false);

	glm::mat4 trans;
	boxShader.use();
	//set model matrix
	//set material
	boxShader.setUniformInt("material.diffuse", 0);
	boxShader.setUniformInt("material.specular", 1);
	boxShader.setUniformFloat("material.shininess", 32.0f);
	
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currTime = glfwGetTime();
		deltaTime = currTime - lastTime;
		lastTime = currTime;

		//clear color buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//process input
		processInput(window);
		//check events
		glfwPollEvents();
		//view matrix and projection matrix
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 proj = glm::perspective(glm::radians(camera.zoom), (GLfloat)WIDTH / HEIGHT, 0.1f, 100.0f);
		//active textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Textures[wood]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Textures[iron]);

		//because we need to wander in the scene,
		//the view matrix and projection(zoom) matrix may changed in every frame
		glBindBuffer(GL_UNIFORM_BUFFER, Uniforms[vpMatirx]);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		
		boxShader.use();
		//spotLight's position and direction, these two vector may changed in every frame
		for (size_t i = 0; i < SpotLightNum; ++i)
		{
			GLuint baseOffset = 64 + PointLightNum * 80 + i * 112;
			sLights[i].position = camera.position;
			sLights[i].direction = camera.front;
			sLights[i].setUniform(Uniforms[lights], baseOffset);
		}
		
		boxShader.setUniformVec3("viewPos", camera.position);
		//draw 10 boxes
		for (size_t i = 0; i < 10; ++i)
		{
			trans = glm::mat4();
			trans = glm::translate(trans, boxPositions[i]);
			float angle = 20.0f * i;
			trans = glm::rotate(trans, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			boxShader.setUniformMat4("model", trans);
			glBindVertexArray(VAOs[box]);
			glDrawArrays(GL_TRIANGLES, 0, boxData.size());
		}
		
		lampShader.use();
		//because we use ubo, so there is no need to set uniform mat4 view and projection
		//see more detail in lamp.vert and box.vert
		//draw 4 point lights
		for (size_t i = 0; i < 4; ++i)
		{
			trans = glm::mat4();
			trans = glm::translate(trans, pointLightPositions[i]);
			trans = glm::scale(trans, glm::vec3(0.1f, 0.1f, 0.1f));
			lampShader.setUniformMat4("model", trans);
			glBindVertexArray(VAOs[lamp]);
			glDrawArrays(GL_TRIANGLES, 0, boxData.size());
		}
		glBindVertexArray(0);

		//swap frame buffer
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

void loadTexture2D(const std::string &imageFile, GLuint &id, bool flip)
{
	glBindTexture(GL_TEXTURE_2D, id);
	//set the wrapping/filtering option
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//load texture image
	GLint imageWidth, imageHeight, nrChannel;
	//if the image is upside down, we need to flip it when loading
	if (flip)
		stbi_set_flip_vertically_on_load(true);
	unsigned char *imageData = stbi_load(imageFile.c_str(), &imageWidth, &imageHeight, &nrChannel, 0);
	if (!imageData)
		std::cerr << "Failed to load texture\n";
	//test the format of image
	std::string format = imageFile.substr(imageFile.rfind('.') + 1);
	if (format == "jpg")
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	if (format == "png")
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);
	//don't forget to free the image
	stbi_image_free(imageData);
}