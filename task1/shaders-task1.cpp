#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "include/GLAUX.LIB")

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "include/GL/glew.h"
#include "GL/freeglut.h"
#include "include/GLM/glm.hpp"
#include "include/GLM/mat3x3.hpp"

#include "include/glm/trigonometric.hpp"

#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glaux.h"
#include <algorithm>
#include <functional>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;
//----------------------------ANGLE
float angle = 5.0f;
//----------------------------SCALE Õ
float scale_x = 0.5f;
//----------------------------SCALE Y
float scale_y = 0.5f;

enum Axis {X,Y,Z};

Axis ax = X;

bool mode = true;

int w, h;

GLuint Program;
GLint  Unif_matr;
GLint  Attrib_vertex;
GLint  Unif_color;
string vsPath = "vertex.shader";
string fsPath = "fragment.shader";

void checkOpenGLerror()
{
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR)
		std::cout << "OpenGl error! - " << gluErrorString(errCode);
}

string loadFile(string path) {
	ifstream fs(path, ios::in);
	if (!fs) cerr << "Cannot open " << path << endl;
	string fsS;
	while (getline(fs, fsS, '\0'))
		cout << fsS << endl;

	return fsS;
}

void initShader() {
	string _f = loadFile(vsPath);
	const char* vsSource = _f.c_str();

	GLuint vShader, fShader;

	vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &vsSource, NULL);
	glCompileShader(vShader);

	_f = loadFile(fsPath);
	const char* fsSource = _f.c_str();


	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fsSource, NULL);
	glCompileShader(fShader);

	//----

	Program = glCreateProgram();
	glAttachShader(Program, vShader);
	glAttachShader(Program, fShader);

	glLinkProgram(Program);

	int link_ok;
	glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) { std::cout << "error attach shaders \n";   return; }


	const char* attr_name = "coord";
	Attrib_vertex = glGetAttribLocation(Program, attr_name);
	if (Attrib_vertex == -1) { cout << "could not bind attrib " << attr_name << endl;   return; }

	const char* unif_name = "matrix";
	Unif_matr = glGetUniformLocation(Program, unif_name);
	if (Unif_matr == -1) { std::cout << "could not bind uniform " << unif_name << std::endl;   return; }

	const char* unif_color = "color";
	Unif_color = glGetUniformLocation(Program, unif_color);
	if (Unif_color == -1) { std::cout << "could not bind uniform " << unif_color << std::endl;   return; }

	checkOpenGLerror();
}

void freeShader() {
	glUseProgram(0);
	glDeleteProgram(Program);
}

void resizeWindow(int width, int height) { glViewport(0, 0, width, height); }

void keyboardDown(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'w':
		scale_x += 0.01;
		scale_y += 0.01;
		break;
	case 's':
		scale_x -= 0.01;
		scale_y -= 0.01;
		break;
	case 'd':
		angle -= 1;
		break;
	case 'a':
		angle += 1;
		break;
	case 'e':
		mode = !mode;
	default:
		break;
	}
	glutPostRedisplay();
}

void render2() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glUseProgram(Program);

	 glm::mat4 scale = { scale_x, 0.0f, 0.0f, 0.0f,
						0.0f, scale_y, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f };

	float a = angle * 3.14f / 180.0f;

	 glm::mat4 rotateX = { 1.0f, 0.0f, 0.0f, 0.0f,
								0.0f, glm::cos(a), -glm::sin(a), 0.0f,
								0.0f, glm::sin(a), glm::cos(a), 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f };

	 glm::mat4 rotateZ = { glm::cos(a), -glm::sin(a), 0.0f, 0.0f,
								glm::sin(a), glm::cos(a), 0.0f, 0.0f,
								0.0f, 0.0f, 1.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f };

	 glm::mat4 rotateY = { glm::cos(a),0.0f, glm::sin(a), 0.0f,
								0.0f, 1.0f, 0.0f, 0.0f,
								-glm::sin(a), 0.0f, glm::cos(a), 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f };

	if (mode)
		switch (ax) {
		case(X):
			glUniformMatrix4fv(Unif_matr, 1, GL_FALSE, &rotateX[0][0]);
			break;
		case(Y):
			glUniformMatrix4fv(Unif_matr, 1, GL_FALSE, &rotateY[0][0]);
			break;
		case(Z):
			glUniformMatrix4fv(Unif_matr, 1, GL_FALSE, &rotateZ[0][0]);
			break;
		}
	else
		glUniformMatrix4fv(Unif_matr, 1, GL_FALSE, &scale[0][0]);

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, 3),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	GLuint MatrixID = glGetUniformLocation(Program, "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	static float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glUniform4fv(Unif_color, 1, red);

	glutSolidTeapot(1);

	glFlush();

	glUseProgram(0);

	checkOpenGLerror();

	glutSwapBuffers();
}


void specialKey(int key, int x, int y) {
	switch ((int)key) {
	case GLUT_KEY_F1:
		ax = X;
		break;
	case GLUT_KEY_F2:
		ax = Y;
		break;
	case GLUT_KEY_F3:
		ax = Z;
		break;
	}
}

void mouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0)
	{
		scale_x += 0.02;
		scale_y += 0.02;
	}
	else
	{
		scale_x -= 0.02;
		scale_y -= 0.02;
	}

	return;
}
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Simple shaders");
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	

	GLenum glew_status = glewInit();
	if (GLEW_OK != glew_status) {
		std::cout << "Error: " << glewGetErrorString(glew_status) << "\n";   return 1;
	}

	if (!GLEW_VERSION_2_0) { std::cout << "No support for OpenGL 2.0 found\n";   return 1; }

	initShader();

	glutReshapeFunc(resizeWindow);
	

	glutSpecialFunc(specialKey);
	glutIdleFunc(render2);

	glutDisplayFunc(render2);
	glutKeyboardFunc(keyboardDown);
	glutMouseWheelFunc(mouseWheel);

	glutMainLoop();

	freeShader();
}