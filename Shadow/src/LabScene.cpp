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
#include <FrameBuffer.h>
#include <SkyBox.h>
#include <ShadowMap.h>

#include <iostream>
#include <cmath>
#include <map>
#include <unordered_map>

//window's width and height
GLuint WIDTH = 1024;
GLuint HEIGHT = 768;

enum Uniform_IDs{ lights, VPmatrix, NumUniforms };
enum Attrib_Ids{ vPostion, vNormal, vTexCoord };

//the Uniform Buffer Objects
GLuint Uniforms[NumUniforms];

glm::vec3 cameraPos(0.0f, 1.0f, 5.0f);
FreeCamera camera(cameraPos);

//this two value were used to control the camera's speed
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

//the first time cursor move into screen
bool firstMouse = true;
//the cursor's position in last frame
GLfloat lastX = GLfloat(WIDTH) / 2;
GLfloat lastY = GLfloat(HEIGHT) / 2;

const GLint PointLightNum = 4;
const GLint SpotLightNum = 1;
const GLint grassNum = 5;
const GLint BoxNum = 2;

const glm::vec3 boxPostions[BoxNum] =
{
	glm::vec3( 0.0f, 0.52f,  2.0f), //to prevent z-fighting, we set y-axis coordinate to 0.52f
	glm::vec3(-0.5f, 1.5f, -3.0f)
};

const glm::vec3 pointLightPositions[PointLightNum] =
{
	glm::vec3( 0.2f, 0.6f,  3.0f),
	glm::vec3( 0.0f, 0.5f,  0.5f),
	glm::vec3( 1.0f, 1.0f,  3.5f),
	glm::vec3(-3.5f, 1.5f, -4.5f)
};

const glm::vec3 spotLightPositions[SpotLightNum] =
{
	glm::vec3(4.0f, 1.0f, 0.0f)
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
	std::cout << "OpenGL 4.2 GO! Let's make some fun!" << std::endl;

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
	Texture2D containerTexture;
	containerTexture.loadFromFile("../../../image/container2.png");
	containerTexture.type = "texture_diffuse";

	Texture2D ironTexture;
	ironTexture.loadFromFile("../../../image/container2_specular.png");
	ironTexture.type = "texture_specular";
	
	Texture2D transparentTexture;
	transparentTexture.loadFromFile("../../../image/blending_transparent_window.png");
	transparentTexture.type = "texture_diffuse";

	Texture2D brickTexture;
	brickTexture.loadFromFile("../../../image/brickwall.jpg");
	brickTexture.type = "texture_diffuse";

	Texture2D woodTexture;
	woodTexture.loadFromFile("../../../image/wood.png");
	woodTexture.type = "texture_diffuse";

	Texture2D brickNormalTexture;
	brickNormalTexture.loadFromFile("../../../image/brickwall_normal.jpg");
	brickNormalTexture.type = "texture_normal";

	std::vector<Texture2D> boxTexture{ containerTexture, ironTexture };
	std::vector<Texture2D> planeTexture{ woodTexture };
	std::vector<Texture2D> transwallTexture{ transparentTexture };
	std::vector<Texture2D> brickwallTexture{ brickTexture, brickNormalTexture };

	/////////////////////////////////////////Objects//////////////////////////////////////////////////
	VertDataLoader vertexLoader;
	vertexLoader.loadData("boxVertexData.txt");
	std::vector<Vertex> boxData = vertexLoader.data;
	Mesh box(boxData, boxTexture);
	Mesh lamp(boxData);

	vertexLoader.loadData("planeVertexData.txt");
	std::vector<Vertex> planeData = vertexLoader.data;
	Mesh plane(planeData, planeTexture);
	Mesh backwall(planeData, brickwallTexture);
	Mesh leftwall(planeData, transwallTexture);
	Mesh rightwall(planeData, transwallTexture);
	Mesh frontwall(planeData, transwallTexture);
	Mesh topwall(planeData, transwallTexture);

	//add reflect map
	Model nanosuit("../../../asset/nanosuit_reflect/nanosuit.obj");

	Model cyborg("../../../asset/cyborg/cyborg.obj");

	SkyBox sky("../../../image/ame_nebula");

	vertexLoader.loadData("frameBuffer.txt");
	std::vector<Vertex> screenData = vertexLoader.data;
	Mesh screen(screenData);
	
	////////////////////////////////////////Shaders/////////////////////////////////////////////////////
	Shader boxShader("box.vert", "box.frag");
	Shader lampShader("lamp.vert", "lamp.frag");
	Shader torchShader("lamp.vert", "torch.frag");
	Shader nanosuitShader("nanosuit.vert", "nanosuit.frag");
	Shader cyborgShader("cyborgNorm.vert", "cyborgNorm.frag");
	Shader planeShader("plane.vert", "plane.frag");
	Shader backwallShader("plane.vert", "brickwall.frag");
	Shader rightwallShader("plane.vert", "transplane.frag");
	Shader leftwallShader("plane.vert", "transplane.frag");
	Shader frontwallShader("plane.vert", "transplane.frag");
	Shader topwallShader("plane.vert", "transplane.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");
	Shader showNormalShader("showNormal.vert", "showNormal.frag", "showNormal.geom");
	Shader depthShader("depth.vert", "depth.frag");
	Shader spotShadowShader("depth.vert", "depth.frag");
	Shader screenShader("framebuffer.vert", "framebuffer.frag");

	//create lights ubo
	glGenBuffers(NumUniforms, Uniforms);
	glBindBuffer(GL_UNIFORM_BUFFER, Uniforms[lights]);
	glBufferData(GL_UNIFORM_BUFFER, 64 + PointLightNum * 80 + SpotLightNum * 112, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, Uniforms[lights]);
	//create view matrix and projection matrix ubo
	glBindBuffer(GL_UNIFORM_BUFFER, Uniforms[VPmatrix]);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, Uniforms[VPmatrix]);

	//////////////////////////////////////////////Lights////////////////////////////////////////////////////
	//directional light
	DirLight dLight
	{
		glm::vec3(-0.6f, -3.0f, -3.5f),   //direction
		glm::vec3(0.02f, 0.02f, 0.02f),   //ambient
		glm::vec3(0.3f, 0.3f, 0.3f),      //diffuse
		glm::vec3(0.5f, 0.5f, 0.5f)       //specular
	};
	//point lights
	std::vector<PointLight> pLights;
	for (size_t i = 0; i < PointLightNum; ++i)
	{
		PointLight pLight
		{
			pointLightPositions[i],
			glm::vec3(0.01f, 0.01f, 0.01f),
			glm::vec3(0.7f, 0.7f, 0.7f),
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
			spotLightPositions[i],         //position
			glm::vec3(0.0f, 0.0f, -1.0f),  //direction
			glm::vec3(0.0f,  0.0f, 0.0f),  //ambient
			glm::vec3(1.0f,  1.0f, 1.0f),  //diffuse
			glm::vec3(1.0f,  1.0f, 1.0f),  //specular
			{ 1.0f, 0.007f, 0.0002f },
			{ glm::cos(glm::radians(25.0f)), glm::cos(glm::radians(35.0f)) }
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
	trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, -1.0f));
	trans = glm::scale(trans, glm::vec3(0.2f, 0.2f, 0.2f));
	nanosuitShader.use();
	nanosuitShader.setUniformMat4("model", trans);
	nanosuitShader.setUniformFloat("material.shininess", 32.0f);

	trans = glm::mat4();
	trans = glm::translate(trans, glm::vec3(3.0f, 0.0f, -3.0f));
	trans = glm::scale(trans, glm::vec3(0.8f, 0.8f, 0.8f));
	cyborgShader.use();
	cyborgShader.setUniformMat4("model", trans);
	cyborgShader.setUniformFloat("material.shininess", 32.0f);

	trans = glm::mat4();
	planeShader.use();
	planeShader.setUniformMat4("model", trans);
	planeShader.setUniformFloat("material.shininess", 32.0f);

	trans = glm::mat4();
	trans = glm::translate(trans, glm::vec3(0.0f, 5.0f, -5.0f));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	backwallShader.use();
	backwallShader.setUniformMat4("model", trans);
	backwallShader.setUniformFloat("material.shininess", 32.0f);

	std::unordered_map<GLuint, Shader> wallHash;

	trans = glm::mat4();
	trans = glm::translate(trans, glm::vec3(5.0f, 5.0f, 0.0f));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	rightwallShader.use();
	rightwallShader.setUniformMat4("model", trans);
	wallHash.insert({ rightwall.VAO, rightwallShader });

	trans = glm::mat4();
	trans = glm::translate(trans, glm::vec3(-5.0f, 5.0f, 0.0f));
	trans = glm::rotate(trans, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	leftwallShader.use();
	leftwallShader.setUniformMat4("model", trans);
	wallHash.insert({ leftwall.VAO, leftwallShader });

	trans = glm::mat4();
	trans = glm::translate(trans, glm::vec3(0.0f, 5.0f, 5.0f));
	trans = glm::rotate(trans, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	frontwallShader.use();
	frontwallShader.setUniformMat4("model", trans);
	wallHash.insert({ frontwall.VAO, frontwallShader });

	trans = glm::mat4();
	trans = glm::translate(trans, glm::vec3(0.0f, 10.0f, 0.0f));
	trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	topwallShader.use();
	topwallShader.setUniformMat4("model", trans);
	wallHash.insert({ topwall.VAO, topwallShader });
	
	trans = glm::mat4();

	///////////////////////////////////////////////////Shadow///////////////////////////////////////////////////
	GLuint shadowX = 1024, shadowY = 1024;
	ShadowMap dirShadow(shadowX, shadowY);
	GLfloat nearPlane = 0.1f, farPlane = 20.0f;
	glm::mat4 dirLightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
	glm::mat4 dirLightView = glm::lookAt(glm::vec3(0.6f, 3.0f, 1.5f), glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 dirLightMatrix = dirLightProjection * dirLightView;
	depthShader.use();
	depthShader.setUniformMat4("lightSpaceMatrix", dirLightMatrix);

	ShadowMap spotShadow(shadowX, shadowY);
	glm::mat4 spotLightProjection = glm::perspective(glm::radians(45.0f), 1.0f, nearPlane, farPlane);
	glm::mat4 spotLightView = glm::lookAt(sLights[0].position + glm::vec3(0.0f, 0.0f, 2.0f), sLights[0].position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 spotLightMatrix = spotLightProjection * spotLightView;
	spotShadowShader.use();
	spotShadowShader.setUniformMat4("lightSpaceMatrix", spotLightMatrix);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FRAMEBUFFER_SRGB);
	///////////////////////////////////////////////////Game Loop////////////////////////////////////////////////
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currTime = glfwGetTime();
		deltaTime = currTime - lastTime;
		lastTime = currTime;

		//process input
		processInput(window);
		//check events
		glfwPollEvents();

		///////////////////////////////////////////////render depthMap////////////////////////////////////////////
		glViewport(0, 0, shadowX, shadowY);
		glBindFramebuffer(GL_FRAMEBUFFER, dirShadow.depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		depthShader.use();
		for (size_t i = 0; i < BoxNum; ++i)
		{
			trans = glm::mat4();
			trans = glm::translate(trans, boxPostions[i]);
			depthShader.setUniformMat4("model", trans);
			box.draw(depthShader);
		}
		for (size_t i = 0; i < PointLightNum; ++i)
		{
			trans = glm::mat4();
			trans = glm::translate(trans, pointLightPositions[i]);
			trans = glm::scale(trans, glm::vec3(0.1f, 0.1f, 0.1f));
			depthShader.setUniformMat4("model", trans);
			lamp.draw(depthShader);
		}
		for (size_t i = 0; i < SpotLightNum; ++i)
		{
			trans = glm::mat4();
			trans = glm::translate(trans, spotLightPositions[i]);
			trans = glm::scale(trans, glm::vec3(0.1f, 0.1f, 0.1f));
			depthShader.setUniformMat4("model", trans);
			lamp.draw(depthShader);
		}
		trans = glm::mat4();
		trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, -1.0f));
		trans = glm::scale(trans, glm::vec3(0.2f, 0.2f, 0.2f));
		depthShader.setUniformMat4("model", trans);
		nanosuit.draw(depthShader);

		trans = glm::mat4();
		trans = glm::translate(trans, glm::vec3(3.0f, 0.0f, -3.0f));
		trans = glm::scale(trans, glm::vec3(0.8f, 0.8f, 0.8f));
		depthShader.setUniformMat4("model", trans);
		cyborg.draw(depthShader);

		trans = glm::mat4();
		depthShader.setUniformMat4("model", trans);
		plane.draw(depthShader);

		trans = glm::mat4();
		trans = glm::translate(trans, glm::vec3(0.0f, 5.0f, -5.0f));
		trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		depthShader.setUniformMat4("model", trans);
		backwall.draw(depthShader);

		glBindFramebuffer(GL_FRAMEBUFFER, spotShadow.depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		spotShadowShader.use();
		trans = glm::mat4();
		trans = glm::translate(trans, glm::vec3(3.0f, 0.0f, -3.0f));
		trans = glm::scale(trans, glm::vec3(0.8f, 0.8f, 0.8f));
		spotShadowShader.setUniformMat4("model", trans);
		cyborg.draw(spotShadowShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		///////////////////////////////////////////////render Scene//////////////////////////////////////////////
		glViewport(0, 0, WIDTH, HEIGHT);
		//clear color buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//view matrix and projection matrix
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 proj = glm::perspective(glm::radians(camera.zoom), (GLfloat)WIDTH / HEIGHT, 0.1f, 100.0f);
		//set ubo VPmatrix
		glBindBuffer(GL_UNIFORM_BUFFER, Uniforms[VPmatrix]);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		boxShader.use();
		boxShader.setUniformMat4("dirLightSpaceMatrix", dirLightMatrix);
		glActiveTexture(GL_TEXTURE2);
		boxShader.setUniformInt("shadowMap", 2);
		glBindTexture(GL_TEXTURE_2D, dirShadow.depthMap);
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

		torchShader.use();
		for (size_t i = 0; i < SpotLightNum; ++i)
		{
			trans = glm::mat4();
			trans = glm::translate(trans, spotLightPositions[i]);
			trans = glm::scale(trans, glm::vec3(0.1f, 0.1f, 0.1f));
			torchShader.setUniformMat4("model", trans);
			lamp.draw(torchShader);
		}

		nanosuitShader.use();
		nanosuitShader.setUniformMat4("dirLightSpaceMatrix", dirLightMatrix);
		nanosuitShader.setUniformVec3("viewPos", camera.position);
		//the cubeMap is the 4th texture in nanosuitShader
		//use it to implement the reflect map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, sky.textureID);
		glActiveTexture(GL_TEXTURE4);
		nanosuitShader.setUniformInt("shadowMap", 4);
		glBindTexture(GL_TEXTURE_2D, dirShadow.depthMap);
		nanosuit.draw(nanosuitShader);

		cyborgShader.use();
		cyborgShader.setUniformMat4("dirLightSpaceMatrix", dirLightMatrix);
		cyborgShader.setUniformVec3("viewPos", camera.position);
		glActiveTexture(GL_TEXTURE2);
		cyborgShader.setUniformInt("shadowMap", 2);
		glBindTexture(GL_TEXTURE_2D, dirShadow.depthMap);
		cyborg.draw(cyborgShader);

		planeShader.use();
		planeShader.setUniformMat4("dirLightSpaceMatrix", dirLightMatrix);
		planeShader.setUniformMat4("spotLightSpaceMatrix", spotLightMatrix);
		glActiveTexture(GL_TEXTURE1);
		planeShader.setUniformInt("dirShadowMap", 1);
		glBindTexture(GL_TEXTURE_2D, dirShadow.depthMap);
		glActiveTexture(GL_TEXTURE2);
		planeShader.setUniformInt("spotShadowMap", 2);
		glBindTexture(GL_TEXTURE_2D, spotShadow.depthMap);
		planeShader.setUniformVec3("viewPos", camera.position);
		plane.draw(planeShader);

		backwallShader.use();
		backwallShader.setUniformMat4("dirLightSpaceMatrix", dirLightMatrix);
		backwallShader.setUniformMat4("spotLightSpaceMatrix", spotLightMatrix);
		glActiveTexture(GL_TEXTURE2);
		backwallShader.setUniformInt("dirShadowMap", 2);
		glBindTexture(GL_TEXTURE_2D, dirShadow.depthMap);
		glActiveTexture(GL_TEXTURE3);
		backwallShader.setUniformInt("spotShadowMap", 3);
		glBindTexture(GL_TEXTURE_2D, spotShadow.depthMap);
		backwallShader.setUniformVec3("viewPos", camera.position);
		backwall.draw(backwallShader);

		/*showNormalShader.use();
		nanosuit.draw(showNormalShader);*/

		//the z-value of skybox is always 1.0f(to know the details, see in shader, I set the z equals to w)
		//so we set depthFunc to less-equal to make skybox pass the depth test
		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		sky.draw(skyboxShader);

		glDepthFunc(GL_LESS);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		std::multimap<float, Mesh> sorted;
		float ld = glm::length(camera.position - glm::vec3(-5.0f, 5.0f, 0.0f));
		sorted.insert({ ld, leftwall });
		float rd = glm::length(camera.position - glm::vec3(5.0f, 5.0f, 0.0f));
		sorted.insert({ rd, rightwall });
		float fd = glm::length(camera.position - glm::vec3(0.0f, 5.0f, 5.0f));
		sorted.insert({ fd, frontwall });
		float td = glm::length(camera.position - glm::vec3(0.0f, 10.0f, 0.0f));
		sorted.insert({ td, topwall });

		for (auto iter = sorted.crbegin(); iter != sorted.crend(); ++iter)
		{
			Shader s = wallHash[(*iter).second.VAO];
			s.use();
			s.setUniformVec3("viewPos", camera.position);
			(*iter).second.draw(s);
		}
		
		glDisable(GL_BLEND);

        /////////////////////////////////////////debug shadow///////////////////////////////////////////
        /*glViewport(0, 0, WIDTH, HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        screenShader.use();
        glBindTexture(GL_TEXTURE_2D, spotShadow.depthMap);
		screen.draw(screenShader);*/


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
	glfwWindowHint(GLFW_SAMPLES, 4);
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