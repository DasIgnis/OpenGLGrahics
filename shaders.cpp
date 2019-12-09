#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "include/GLAUX.LIB")

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "include/glaux.h"


int w = 0, h = 0;

GLuint Program;

GLint Unif_texture1;
GLint Unif_texture2;
GLint Unif_intens;

AUX_RGBImageRec* gift_img;
GLuint gift_tex;

AUX_RGBImageRec* santa_img;
GLuint santa_tex;

GLuint VAO, VBO, EBO;

GLfloat RIntens = 0.f, GIntens = 0.f, BIntens = 0.f;
float textuteIntens = 0.0;

void LoadAUXTextures() {
	gift_img = auxDIBImageLoad("sources\\gift.bmp");
	glGenTextures(1, &gift_tex);
	glBindTexture(GL_TEXTURE_2D, gift_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		gift_img->sizeX,
		gift_img->sizeY,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		gift_img->data);

	santa_img = auxDIBImageLoad("sources\\santa.bmp");
	glGenTextures(1, &santa_tex);
	glBindTexture(GL_TEXTURE_2D, santa_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		santa_img->sizeX,
		santa_img->sizeY,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		santa_img->data);
}

void resizeWindow(int width, int height) {
	glViewport(0, 0, width, height);
}

void checkOpenGLerror() {
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL error!" << gluErrorString(errCode);
	}
}

void initShader() {
	const char* vsSource =
		"#version 330\n"
		"layout (location = 0) in vec2 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"layout (location = 2) in vec2 aTexCoord;\n"
		"out vec3 ourColor;\n"
		"out vec2 TexCoord;\n"
		"void main() {\n"
		"	gl_Position = vec4(aPos, 0.0, 1.0);\n"
		"	ourColor = aColor;\n"
		"	TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
		"}\n";

	const char* fsSource =
		"#version 330\n"
		"out vec4 FragColor;\n"
		"in vec3 ourColor;\n"
		"in vec2 TexCoord;"
		"uniform sampler2D texture1;\n"
		"uniform sampler2D texture2;\n"
		"uniform float intens;\n"
		"void main() {\n"
		"	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), intens)*vec4(ourColor, 1.0f);\n"
		"}\n";

	GLuint fShader;
	GLuint vShader;

	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fsSource, NULL);
	glCompileShader(fShader);

	vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &vsSource, NULL);
	glCompileShader(vShader);

	Program = glCreateProgram();
	glAttachShader(Program, vShader);
	glAttachShader(Program, fShader);

	glLinkProgram(Program);

	int link_ok;
	glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);

	if (!link_ok) {
		char* log = new char[1024];
		GLsizei length = 0;
		glGetProgramInfoLog(Program, 1024, &length, log);
		std::cout << "error attach shaders " << log << " \n";
		return;
	}

	const char* unif_name = "texture1";
	Unif_texture1 = glGetUniformLocation(Program, unif_name);
	if (Unif_texture1 == -1) {
		std::cout << "could not bind uniform " << unif_name << std::endl;
		return;
	}

	const char* unif_name2 = "texture2";
	Unif_texture2 = glGetUniformLocation(Program, unif_name2);
	if (Unif_texture2 == -1) {
		std::cout << "could not bind uniform " << unif_name2 << std::endl;
		return;
	}

	const char* unif_name3 = "intens";
	Unif_intens = glGetUniformLocation(Program, unif_name3);
	if (Unif_intens == -1) {
		std::cout << "could not bind uniform " << unif_name3 << std::endl;
		return;
	}
	checkOpenGLerror();
}

void freeShader() {
	glUseProgram(0);
	glDeleteProgram(Program);
}

void setupBuffers() {
	float vertices[] = {
			-0.5f,  0.5f, 1.0f, 1.0f - RIntens, 1.0f - RIntens, 0.0f, 0.0f,
			 0.5f,  0.5f, 1.0f - GIntens, 1.0f, 1.0f - GIntens, 1.0f, 0.0f,
			 0.5f, -0.5f, 1.0f - BIntens, 1.0f - BIntens, 1.0f, 1.0f, 1.0f,
			-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};

	//Настройка VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Настройка VBO
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Настройка EBO
	glGenBuffers(1, &EBO);
	GLuint ebo_data[] = { 0, 1, 2, 2, 3, 0 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ebo_data), ebo_data, GL_STATIC_DRAW);
}

void render2() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	setupBuffers();

	glUseProgram(Program);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gift_tex);
	glUniform1i(Unif_texture1, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, santa_tex);
	glUniform1i(Unif_texture2, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glUniform1f(Unif_intens, textuteIntens);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glFlush();

	glUseProgram(0);

	checkOpenGLerror();
	glutSwapBuffers();
}

void keyboardDown(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'a':
		RIntens = (RIntens - 0.1f > 0.0f ? RIntens - 0.1f : 0.0f);
		GIntens = (GIntens - 0.1f > 0.0f ? GIntens - 0.1f : 0.0f);
		BIntens = (BIntens - 0.1f > 0.0f ? BIntens - 0.1f : 0.0f);
		break;
	case 'd':
		RIntens = (RIntens + 0.1f < 1.0f ? RIntens + 0.2f : 1.0f);
		GIntens = (GIntens + 0.1f < 1.0f ? GIntens + 0.2f : 1.0f);
		BIntens = (BIntens + 0.1f < 1.0f ? BIntens + 0.2f : 1.0f);
		break;
	case 'q':
		textuteIntens = (textuteIntens - 0.1 > 0.0 ? textuteIntens - 0.1 : 0.0);
		break;
	case 'e':
		textuteIntens = (textuteIntens + 0.1 < 1.0 ? textuteIntens + 0.1 : 1.0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {

	setlocale(LC_ALL, "RUSSIAN");

	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutCreateWindow("OpenGL");

	LoadAUXTextures();

	glClearColor(0, 0, 0, 0);

	GLenum glew_status = glewInit();
	if (GLEW_OK != glew_status) {
		std::cout << "Error" << glewGetErrorString(glew_status) << "\n";
		return 1;
	}

	if (!GLEW_VERSION_2_0) {
		std::cout << "No supp for OPGL2.0\n";
		return 1;
	}

	setupBuffers();
	initShader();

	glutReshapeFunc(resizeWindow);
	glutDisplayFunc(render2);

	glutKeyboardFunc(keyboardDown);

	glutMainLoop();

	freeShader();
	return 0;
}