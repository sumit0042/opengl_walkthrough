#include<GL/glew.h>     // use this before GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<iostream>
#include<fstream>
#include<string>
#include<tuple>
#include<Windows.h>

#include "Shader.h"
#include "Camera.h"

using namespace std;

GLuint loadBMP_custom(const char* imagepath);
tuple< GLuint, GLuint, GLuint> CreateFrameBuffer();


/*=======================================================================================================================
			DATA PREPARATION
=======================================================================================================================*/

const int no_rects = 12;

struct Vertex
{
	float x, y, z;
	float tx = 0.0; float ty = 0.0;
};

glm::mat4x3 faces[no_rects];
Vertex c[no_rects][4];

void create_faces_data()
{
	glm::mat4x3 frontBndry = {
		{-0.9f,-0.21f, 0.9f},
		{-0.9f, 0.21f, 0.9f},
		{ 0.9f, 0.21f, 0.9f},
		{ 0.9f,-0.21f, 0.9f}
	};

	glm::mat4x3 backBndry = {
		{-0.9f,-0.21f,-0.9f},
		{-0.9f, 0.21f,-0.9f},
		{ 0.9f, 0.21f,-0.9f},
		{ 0.9f,-0.21f,-0.9f},
	};

	glm::mat4x3 rightBdry = {
		{ 0.9f,-0.21f, 0.9f},
		{ 0.9f, 0.21f, 0.9f},
		{ 0.9f, 0.21f,-0.9f},
		{ 0.9f,-0.21f,-0.9f}
	};

	glm::mat4x3 leftBdry = {
		{-0.9f,-0.21f, 0.9f},
		{-0.9f, 0.21f, 0.9f},
		{-0.9f, 0.21f,-0.9f},
		{-0.9f,-0.21f,-0.9f}
	};

	glm::mat4x3 ground = {
		{ 0.9f,-0.21f, 0.9f},
		{ 0.9f,-0.21f,-0.9f},
		{-0.9f,-0.21f,-0.9f},
		{-0.9f,-0.21f, 0.9f}
	};

	glm::mat4x3 ceiling = {
		{ 0.9f, 0.21f, 0.9f},
		{ 0.9f, 0.21f,-0.9f},
		{-0.9f, 0.21f,-0.9f},
		{-0.9f, 0.21f, 0.9f}
	};

	glm::mat4x3 frontFace = {
		{-0.2f,-0.2f, 0.2f},
		{-0.2f, 0.2f, 0.2f},
		{ 0.2f, 0.2f, 0.2f},
		{ 0.2f,-0.2f, 0.2f}
	};

	glm::mat4x3 backFace = {
		{-0.2f,-0.2f,-0.2f},
		{-0.2f, 0.2f,-0.2f},
		{ 0.2f, 0.2f,-0.2f},
		{ 0.2f,-0.2f,-0.2f},
	};

	glm::mat4x3 topFace = {
		{ 0.2f, 0.2f,-0.2f},
		{-0.2f, 0.2f,-0.2f},
		{-0.2f, 0.2f, 0.2f},
		{ 0.2f, 0.2f, 0.2f}
	};

	glm::mat4x3 bottomFace = {
		{-0.2f,-0.2f, 0.2f},
		{-0.2f,-0.2f,-0.2f},
		{ 0.2f,-0.2f,-0.2f},
		{ 0.2f,-0.2f, 0.2f},
	};

	glm::mat4x3 rightFace = {
		{ 0.2f, 0.2f, 0.2f},
		{ 0.2f, 0.2f,-0.2f},
		{ 0.2f,-0.2f,-0.2f},
		{ 0.2f,-0.2f, 0.2f},
	};

	glm::mat4x3 leftFace = {
		{-0.2f, 0.2f, 0.2f},
		{-0.2f,-0.2f, 0.2f},
		{-0.2f,-0.2f,-0.2f},
		{-0.2f, 0.2f,-0.2f},
	};

	faces[0] = frontFace;
	faces[1] = backFace;
	faces[2] = leftFace;
	faces[3] = bottomFace;
	faces[4] = rightFace;
	faces[5] = topFace;
	faces[6] = ground;
	faces[7] = ceiling;
	faces[8] = leftBdry;
	faces[9] = rightBdry;
	faces[10] = frontBndry;
	faces[11] = backBndry;

	for (size_t i = 0; i < no_rects; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			c[i][j].x = faces[i][j].x;
			c[i][j].y = faces[i][j].y;
			c[i][j].z = faces[i][j].z;
			switch (j)
			{
			case 0:
			{
				c[i][j].tx = 0.0f;
				c[i][j].ty = 0.0f;
				break;
			}
			case 1:
			{
				c[i][j].tx = 0.0f;
				c[i][j].ty = 1.0f;
				break;
			}
			case 2:
			{
				c[i][j].tx = 1.0f;
				c[i][j].ty = 1.0f;
				break;
			}
			case 3:
			{
				c[i][j].tx = 1.0f;
				c[i][j].ty = 0.0f;
				break;
			}
			default:
				break;
			}
		}
	}
}


/*=======================================================================================================================
			CAMERA AND INPUTS
=======================================================================================================================*/

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 0.8f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}



/*=======================================================================================================================
			PROGRAM AND DATA BINDING
=======================================================================================================================*/

GLuint create_program()
{
	GLuint program = glCreateProgram();
	shaderAttach(program, GL_VERTEX_SHADER, vertexShaderSrc);
	shaderAttach(program, GL_FRAGMENT_SHADER, fragmentShaderSrc);
	glLinkProgram(program);
	glValidateProgram(program);
	return program;
}

void bind_texture(GLuint program)
{
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	GLuint texID = loadBMP_custom("..\\..\\..\\assets\\morty.bmp");
	glActiveTexture(GL_TEXTURE1);
	GLuint texID2 = loadBMP_custom("..\\..\\..\\assets\\stones.bmp");

	GLint locScale2 = glGetUniformLocation(program, "sampler1");
	glUniform1i(locScale2, 0);

	GLint locScale3 = glGetUniformLocation(program, "sampler2");
	glUniform1i(locScale3, 1);

}

void bind_vert_n_texel(GLuint program)
{
	GLuint posAttrib = glGetAttribLocation(program, "pos");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	posAttrib = glGetAttribLocation(program, "texco");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}



int main()
{
	GLFWwindow* window;

	// Initialize the library 
	if (!glfwInit())
		return -1;

	// Create a windowed mode window and its OpenGL context 
	window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, 800, 600);
	// Make the window's context current 
	glfwMakeContextCurrent(window);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	create_faces_data();

	glewInit();

	GLuint program[no_rects];

	GLuint g;
	glGenBuffers(1, &g);
	glBindBuffer(GL_ARRAY_BUFFER, g);
	glBufferData(GL_ARRAY_BUFFER, sizeof(c), c, GL_STATIC_DRAW);

	glEnable(GL_DEPTH_TEST);

	for (size_t i = 0; i < no_rects; i++)
	{
		program[i] = create_program();
		glUseProgram(program[i]);
		
		bind_vert_n_texel(program[i]);
		bind_texture(program[i]);
	}

	auto fb = CreateFrameBuffer();
	glBindFramebufferEXT(GL_FRAMEBUFFER, get<0>(fb));////////////////------------------

	bool fbl = false;
	bool toggle = true;
	// Loop until the user closes the window 
	while (!glfwWindowShouldClose(window))
	{
		glEnable(GL_DEPTH_TEST);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (size_t i = 0; i < no_rects; i++)
		{
			glUseProgram(program[i]);

			if (toggle && fbl)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, get<1>(fb));
				GLint locScale2 = glGetUniformLocation(program[i], "sampler1");
				glUniform1i(locScale2, 0);
				toggle = false;
			}

			glm::mat4 view = camera.GetViewMatrix();
			GLint viewLoc = glGetUniformLocation(program[i], "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

			glm::mat4 pro = glm::mat4(1.0f);
			pro = glm::perspective(glm::radians(camera.Zoom), 1280.0f / 720.0f, 0.01f, 100.0f);
			GLint projectionLoc = glGetUniformLocation(program[i], "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(pro));

			glDrawArrays(GL_TRIANGLE_FAN, i*4, 4);
			glUseProgram(0);

		}
		fbl = true;
		glBindFramebufferEXT(GL_FRAMEBUFFER, 0);////////////////------------------

		glfwSwapBuffers(window);

		/* Swap front and back buffers */
		

		/* Poll for and process events */
		glfwPollEvents();
	}

	for (size_t i = 0; i < 6; i++)
	{
		glDeleteProgram(program[i]);
	}

	glUseProgram(0);
	glDeleteVertexArrays(1, &g);
	glfwTerminate();
	return 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

GLuint loadBMP_custom(const char* imagepath)
{
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char* data;

	FILE* file = fopen(imagepath, "rb");
	if (!file)
	{
		printf("Image could not be opened\n"); return 0;
	}
	if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return false;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}

	// Read ints from the byte array
	dataPos = *(int*) & (header[0x0A]);
	imageSize = *(int*) & (header[0x22]);
	width = *(int*) & (header[0x12]);
	height = *(int*) & (header[0x16]);
	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way
	// Create a buffer
	data = new unsigned char[imageSize];
	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);
	//Everything is in memory now, the file can be closed
	fclose(file);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	delete[] data;
	return textureID;
}


tuple< GLuint, GLuint, GLuint> CreateFrameBuffer()
{
	GLuint color_tex = 0, fb = 0, depth_tex = 0;
	//RGBA8 2D texture, 24 bit depth texture, 256x256
	glGenTextures(1, &color_tex);
	glBindTexture(GL_TEXTURE_2D, color_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//NULL means reserve texture memory, but texels are undefined
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1300, 650, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	glGenTextures(1, &depth_tex);
	glBindTexture(GL_TEXTURE_2D, depth_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	//NULL means reserve texture memory, but texels are undefined
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 1300, 650, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	//-------------------------
	glGenFramebuffersEXT(1, &fb);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
	//Attach 2D texture to this FBO
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex, 0/*mipmap level*/);
	//-------------------------
	//Attach depth texture to FBO
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depth_tex, 0/*mipmap level*/);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return tuple< GLuint, GLuint, GLuint>(fb, color_tex, depth_tex);
}

//----------------------------------------------------------------------------------------------------------------------