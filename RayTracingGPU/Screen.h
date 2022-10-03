#pragma once
#ifndef Screen_h
#define Screen_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>

const float ScreenVertices[] = {
	//λ������(x,y)     //��������

	//������1
	-1.0f, 1.0f,   0.0f, 1.0f, //���Ͻ� 
	-1.0f, -1.0f,  0.0f, 0.0f, //���½�
	1.0f, -1.0f,   1.0f, 0.0f, //���½�

	//������2
	-1.0f,  1.0f,  0.0f, 1.0f, //���Ͻ�
	1.0f, -1.0f,   1.0f, 0.0f, //���½�
	1.0f,  1.0f,   1.0f, 1.0f  //���Ͻ�
};

class Screen {
public:
	void Init() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenVertices), ScreenVertices, GL_STATIC_DRAW);
		// λ��
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// ����
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Unbind VAO
		glBindVertexArray(0);
	}
	void Draw() {
		// ��VAO����ʼ����
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
private:
	unsigned int VBO, VAO;
};


class ScreenFBO {
public:
	ScreenFBO() {}
	// framebuffer����
	unsigned int framebuffer;
	// ��ɫ��������
	unsigned int textureColorbuffer;
	// ��Ⱥ�ģ�帽����renderbuffer object
	unsigned int rbo;
	void configuration(int SCR_WIDTH, int SCR_HEIGHT) {
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		// ����ɫ����
		glGenTextures(1, &textureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			// û����ȷʵ�֣��򱨴�
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		}
		// �󶨵�Ĭ��FrameBuffer
		unBind();
	}

	void Bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glDisable(GL_DEPTH_TEST);
	}

	void unBind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void BindAsTexture() {
		// ��Ϊ��0������
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	}

	void Delete() {
		// ɾ��
		unBind();
		glDeleteFramebuffers(1, &framebuffer);
		glDeleteTextures(1, &textureColorbuffer);
	}

};


class RenderBuffer {
public:
	void Init(int SCR_WIDTH, int SCR_HEIGHT) {
		fbo[0].configuration(SCR_WIDTH, SCR_HEIGHT);
		fbo[1].configuration(SCR_WIDTH, SCR_HEIGHT);
		currentIndex = 0;
	}
	// fbo[0]Ϊ��Ⱦ����ǰ֡������
	void setCurrentBuffer(int LoopNum) {
		int histIndex = LoopNum % 2;
		int curIndex = (histIndex == 0 ? 1 : 1);

		fbo[curIndex].Bind();
		fbo[curIndex].BindAsTexture();
	}
	void setCurrentAsTexture(int LoopNum) {
		int histIndex = LoopNum % 2;
		int curIndex = (histIndex == 0 ? 1 : 1);
		fbo[curIndex].BindAsTexture();
	}

	void Delete() {
		fbo[0].Delete();
		fbo[1].Delete();
	}
private:
	// ������Ⱦ��ǰ֡������
	int currentIndex;
	ScreenFBO fbo[2];
};


#endif






