#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <stb_image.h>

#include <Shader.h>
#include <Camera.h>
#include <Light.h>
#include <VertexDataLoader.h>
#include <Model.h>
#include <Texture2D.h>

#include <iostream>
#include <cmath>

//window's width and height
GLuint WIDTH = 800;
GLuint HEIGHT = 600;

enum Uniform_IDs{ lights, VPmatrix, NumUniforms };
enum Attrib_Ids{ vPostion, vNormal, vTexCoord };

GLuint Uniforms[NumUniforms];

glm::vec3 cameraPos(0.0f, 1.0f, 5.0f);
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

const GLint BoxNum = 2;

const glm::vec3 boxPostions[BoxNum] =
{
	glm::vec3( 0.0f, 0.5f,  2.0f),
	glm::vec3(-2.0f, 1.5f, -1.5f)
};

const glm::vec3 pointLightPositions[PointLightNum] =
{
	glm::vec3( 0.2f, 0.6f,  3.0f),
	glm::vec3( 0.0f, 0.5f,  0.5f),
	glm::vec3( 1.0f, 1.0f,  3.5f),
	glm::vec3(-3.5f, 1.5f, -1.5f)
};

const glm::vec3 spotLightPositions[SpotLightNum] =
{
	glm::vec3(0.0f, 4.5f, 0.0f)
};

//initialize GLFW window option
void initWindowOption();
//process input
void processInput(GLFWwindow *window);
//frame buffer size callback
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void mouseCallback(GLFWwindow *window, double x, double y);
void scrollCallback(GLFWwindow *window, double x, double y);

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

	///////////////////////////////////////////Texture/////////////////////////////////////////////////
	Texture2D woodTexture;
	woodTexture.loadFromFile("../../../image/container2.png");
	woodTexture.type = "texture_diffuse";

	Texture2D ironTexture;
	ironTexture.loadFromFile("../../../image/container2_specular.png");
	ironTexture.type = "texture_specular";
	std::vector<Texture2D> boxTexture{ woodTexture, ironTexture };

	Texture2D brickTexture;
	brickTexture.loadFromFile("../../../image/wall.jpg");
	brickTexture.type = "texture_diffuse";
	std::vector<Texture2D> planeTexture{ brickTexture };

	/////////////////////////////////////////Objects//////////////////////////////////////////////////
	VertDataLoader vertexLoader;
	vertexLoader.loadData("boxVertexData.txt");
	std::vector<Vertex> boxData = vertexLoader.data;
	Mesh box(boxData, std::vector<GLuint>(), boxTexture);
	Mesh lamp(boxData, std::vector<GLuint>(), std::vector<Texture2D>());

	vertexLoader.loadData("planeVertexData.txt");
	std::vector<Vertex> planeData = vertexLoader.data;
	Mesh plane(planeData, std::vector<GLuint>(), planeTexture);

	Model nanosuit("../../../asset/nanosuit/nanosuit.obj");
	
	////////////////////////////////////////Shaders/////////////////////////////////////////////////////
	Shader boxShader("box.vert", "box.frag");
	Shader lampShader("lamp.vert", "lamp.frag");
	Shader nanosuitShader("nanosuit.vert", "nanosuit.frag");
	Shader planeShader("plane.vert", "plane.frag");


	//create lights ubo
	glGenBuffers(NumUniforms, Uniforms);
	glBindBuffer(GL_UNIFORM_BUFFER, Uniforms[lights]);
	glBufferData(GL_UNIFORM_BUFFER, 64 + PointLightNum * 80 + SpotLightNum * 112, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, Uniforms[lights]);

	glBindBuffer(GL_UNIFORM_BUFFER, Uniforms[VPmatrix]);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, Uniforms[VPmatrix]);

	//////////////////////////////////////////////Lights////////////////////////////////////////////////////
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
			spotLightPositions[i],
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f,  0.0f, 0.0f),
			glm::vec3(1.0f,  1.0f, 1.0f),
			glm::vec3(1.0f,  1.0f, 1.0f),
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
	for (size_t i = 0; i < SpotLightNum; ++i)
	{
		GLuint baseOffset = 64 + PointLightNum * 80 + i * 112;
		sLights[i].setUniform(Uniforms[lights], baseOffset);
	}

	//////////////////////////////////////////////Translates//////////////////////////////////////////////
	glm::mat4 trans;
	boxShader.use();
	boxShader.setUniformFloat("material.shininess", 32.0f);

	trans = glm::mat4();
	trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, 0.0f));
	trans = glm::scale(trans, glm::vec3(0.2f, 0.2f, 0.2f));
	nanosuitShader.use();
	nanosuitShader.setUniformMat4("model", trans);
	nanosuitShader.setUniformFloat("material.shininess", 32.0f);

	trans = glm::mat4();
	trans = glm::scale(trans, glm::vec3(2.0f, 2.0f, 2.0f));
	planeShader.use();
	planeShader.setUniformMat4("model", trans);

	glEnable(GL_DEPTH_TEST);

	///////////////////////////////////////////////////Game Loop////////////////////////////////////////////////
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

		glBindBuffer(GL_UNIFORM_BUFFER, Uniforms[VPmatrix]);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		boxShader.use();
		for (size_t i = 0; i < BoxNum; ++i)
		{
			trans = glm::mat4();
			trans = glm::translate(trans, boxPostions[i]);
			boxShader.setUniformMat4("model", trans);
			boxShader.setUniformVec3("viewPos", camera.position);
			box.draw(boxShader);
		}
		
		lampShader.use();
		//draw 4 point lights
		for (size_t i = 0; i < PointLightNum; ++i)
		{
			trans = glm::mat4();
			trans = glm::translate(trans, pointLightPositions[i]);
			trans = glm::scale(trans, glm::vec3(0.1f, 0.1f, 0.1f));
			lampShader.setUniformMat4("model", trans);
			lamp.draw(lampShader);
		}

		nanosuitShader.use();
		nanosuitShader.setUniformVec3("viewPos", camera.position);
		nanosuit.draw(nanosuitShader);

		planeShader.use();
		planeShader.setUniformVec3("viewPos", camera.position);
		plane.draw(planeShader);

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