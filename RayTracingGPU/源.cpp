#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<shader.h>

#include <iostream>

#include "Screen.h"
#include "Camera.h"
#include "Tool.h"
#include "ObjectTexture.h"
#include "Model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

timeRecord timerecord;
Camera cam(SCR_WIDTH, SCR_HEIGHT);

RenderBuffer screenBuffer;
BVHTree bvhTree;
ObjectTexture ObjTex;

int main() {
	// GLFW��ʼ��
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// ����GLFW����
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "RayTracer", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//// ���ڲ�����꣬����ʾ���
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// �������е�OpenGL����ָ��
	gladLoadGL();

	// ������ɫ��
	Shader ourShader("./shader/RayTracerShader.vert", "./shader/RayTracerShader.frag");
	Shader ScreenShader("./shader/ScreenShader.vert", "./shader/Screenshader.frag");

	Model dragon("./dragon.obj");
	ourShader.use();
	getTexture(dragon.meshes, ourShader, ObjTex, bvhTree, 0.04, glm::vec3(0.0, -0.2, 0.0));

	Screen screen;
	screen.Init();

	screenBuffer.Init(SCR_WIDTH, SCR_HEIGHT);
	// ��Ⱦ��ѭ��

	BVHTest(bvhTree, cam);
	bvhTree.releaseAll();
	while (!glfwWindowShouldClose(window)) {
		// ÿ֡ʱ��
		timerecord.updateTime();

		// ����
		processInput(window);
		
		cam.LoopIncrease();
		// ��Ⱦ
		{
			screenBuffer.setCurrentBuffer(cam.LoopNum);

			ObjTex.setTex(ourShader);

			// ������ɫ��
			ourShader.use();

			ourShader.setInt("historyTexture", 0);
			ourShader.setVec3("camera.camPos", cam.cameraPos);
			ourShader.setVec3("camera.front", cam.cameraFront);
			ourShader.setVec3("camera.right", cam.cameraRight);
			ourShader.setVec3("camera.up", cam.cameraUp);
			ourShader.setFloat("camera.halfH", cam.halfH);
			ourShader.setFloat("camera.halfW", cam.halfW);
			ourShader.setInt("camera.LoopNum", cam.LoopNum);
			ourShader.setVec3("camera.leftbottom", cam.LeftBottomCorner);

			ourShader.setFloat("randOrigin", 674764.0f * (GetCPURandom() + 1.0f));

			/*ourShader.setFloat("sphere[0].radius", 0.5);
			ourShader.setVec3("sphere[0].center", glm::vec3(0.0, 0.0, -1.0));
			ourShader.setInt("sphere[0].materialIndex", 0);
			ourShader.setVec3("sphere[0].albedo", glm::vec3(0.8, 0.7, 0.2));
			
			ourShader.setFloat("sphere[1].radius", 0.5);
			ourShader.setVec3("sphere[1].center", glm::vec3(1.0, 0.0, -1.0));
			ourShader.setInt("sphere[1].materialIndex", 1);
			ourShader.setVec3("sphere[1].albedo", glm::vec3(0.2, 0.7, 0.6));
			
			ourShader.setFloat("sphere[2].radius", 0.5);
			ourShader.setVec3("sphere[2].center", glm::vec3(-1.0, 0.0, -1.0));
			ourShader.setInt("sphere[2].materialIndex", 1);
			ourShader.setVec3("sphere[2].albedo", glm::vec3(0.1, 0.3, 0.7));
			
			ourShader.setFloat("sphere[3].radius", 0.5);
			ourShader.setVec3("sphere[3].center", glm::vec3(0.0, 0.0, 0.0));
			ourShader.setInt("sphere[3].materialIndex", 0);
			ourShader.setVec3("sphere[3].albedo", glm::vec3(0.9, 0.9, 0.9));

			ourShader.setVec3("tri[0].v0", glm::vec3(2.0, -0.5, 2.0));
			ourShader.setVec3("tri[0].v1", glm::vec3(-2.0, -0.5, -2.0));
			ourShader.setVec3("tri[0].v2", glm::vec3(-2.0, -0.5, 2.0));
			
			ourShader.setVec3("tri[1].v0", glm::vec3(2.0, -0.5, 2.0));
			ourShader.setVec3("tri[1].v1", glm::vec3(-2.0, -0.5, -2.0));
			ourShader.setVec3("tri[1].v2", glm::vec3(2.0, -0.5, -2.0));*/

			// �����θ�ֵ
			float floorHfW = 1.0, upBias = -0.22;
			ourShader.setVec3("triFloor[0].v0", glm::vec3(-floorHfW, upBias, floorHfW));
			ourShader.setVec3("triFloor[0].v1", glm::vec3(-floorHfW, upBias, -floorHfW));
			ourShader.setVec3("triFloor[0].v2", glm::vec3(floorHfW, upBias, floorHfW));
		
			ourShader.setVec3("triFloor[1].v0", glm::vec3(floorHfW, upBias, floorHfW));
			ourShader.setVec3("triFloor[1].v1", glm::vec3(-floorHfW, upBias, -floorHfW));
			ourShader.setVec3("triFloor[1].v2", glm::vec3(floorHfW, upBias, -floorHfW));

			screen.Draw();
		}

		// ��Ⱦ��Ĭ��Buffer��
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			ScreenShader.use();
			screenBuffer.setCurrentAsTexture(cam.LoopNum);
			// screenBuffer�󶨵���������Ϊ����0��������������Ƭ����ɫ���е�screenTextureΪ����0
			ScreenShader.setInt("screenTexture", 0);
			screen.Draw();
		}

		// ����Buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// ������ֹ
	glfwTerminate();
	return 0;
}

// ��������
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.ProcessKeyboard(FORWARD, timerecord.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.ProcessKeyboard(BACKWARD, timerecord.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.ProcessKeyboard(LEFT, timerecord.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.ProcessKeyboard(RIGHT, timerecord.deltaTime);
}

// �����ڳߴ�仯
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	cam.updateScreenRatio(SCR_WIDTH, SCR_HEIGHT);
	glViewport(0, 0, width, height);
}

// ����¼���Ӧ
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	cam.updateCameraFront(xpos, ypos);
}

// ����fov
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	cam.updateFov(static_cast<float>(yoffset));
}


