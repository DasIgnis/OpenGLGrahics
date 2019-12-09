#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#include <GL/glew.h>
#pragma comment(lib, "include/GLAUX.LIB")

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <math.h>
#include <iomanip>
#include <string>
#include <fstream>
#include <streambuf>
#include <map>
#include <vector>
#include "GL/freeglut.h"
#include "include/glaux.h"


//! Переменные с индентификаторами ID 
//! ID шейдерной программы 
GLuint Program, program2;
//! ID юниформ переменной цвета 
GLint  ucolor, ucolor1, ucolor2, udir, uwidth;
double rotate_z = 0;

int option_list = 0;
char option_char = 'z';
std::vector<std::map<char, int>> options, speeds;


void setup_options()
{
	option_list = 0;
	option_char = 'z';
	options.clear();
	speeds.clear();
	
	for (int i = 0; i < 3; i++)
	{
		options.push_back(std::map<char, int>());
		speeds.push_back(std::map<char, int>());
		options[i]['z'] = 0;
		options[i]['y'] = 0;
		options[i]['z'] = 0;
		options[i]['r'] = 0;
		options[i]['g'] = 0;
		options[i]['b'] = 0;

		speeds[i]['z'] = 5;
		speeds[i]['y'] = 5;
		speeds[i]['z'] = 5;
		speeds[i]['r'] = 5;
		speeds[i]['g'] = 5;
		speeds[i]['b'] = 5;
	}
	options[1]['r'] = 255;
	options[2]['g'] = 255;


	options[1]['w'] = 50;
	speeds[1]['w'] = 5;
	options[1]['d'] = 1;
	speeds[1]['d'] = 1;
}

//! Проверка ошибок OpenGL, если есть то вывод в консоль тип ошибки 
void checkOpenGLerror() {
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR)
		std::cout << "OpenGl error! - " << gluErrorString(errCode);
}

//! Инициализация шейдеров 
GLuint initShader(std::string filename)
{
	std::ifstream t(filename);
	std::string str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	const char* fsSource = str.c_str();


	//! Переменные для хранения идентификаторов шейдеров 
	GLuint fShader;
	//! Создаем фрагментный шейдер
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	//! Передаем исходный код 
	glShaderSource(fShader, 1, &fsSource, NULL);
	//! Компилируем шейдер
	glCompileShader(fShader);

	//! Создаем программу и прикрепляем шейдеры к ней
	GLuint Program = glCreateProgram();
	glAttachShader(Program, fShader);

	//! Линкуем шейдерную программу
	glLinkProgram(Program); 

	//! Проверяем статус сборки 
	int link_ok;
	glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
	if (!link_ok)
	{
		std::cout << "error attach shaders \n";
		return -1;
	}

	return Program;
}

GLint getUniform(std::string unif_name, GLuint Program)
{
	//! Вытягиваем ID юниформ  	
	GLint result = glGetUniformLocation(Program, unif_name.c_str());
	if (result == -1)
	{
		std::cout << "could not bind uniform " << unif_name << std::endl;
		return -1;
	}
	checkOpenGLerror();
	return result;
}


//! Освобождение шейдеров 
void freeShader()
{
	//! Передавая ноль, мы отключаем шейдрную программу  	
	glUseProgram(0);
	//! Удаляем шейдерную программу  	
	glDeleteProgram(Program);
}
void resizeWindow(int width, int height)
{
	glViewport(0, 0, width, height);
}

//! Отрисовка 
void render2()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glPushMatrix();
	
	glRotatef(options[0]['z'], 0.0, 0.0, 1.0);
	glRotatef(options[0]['y'], 0.0, 1.0, 0.0);
	glRotatef(options[0]['x'], 1.0, 0.0, 0.0);

	//! Устанавливаем шейдерную программу текущей
	glUseProgram(Program); 
	////! Передаем юниформ в шейдер
	glUniform4f(ucolor, options[0]['r'] / 255.f, options[0]['g'] / 255.f, options[0]['b'] / 255.f, 1.f);

	glTranslatef(0, -.5f, 0);

	glBegin(GL_QUADS);
	glColor3f(1.0, 0.0, 0.0); glVertex2f(-0.5f, -0.5f);
	glColor3f(0.0, 1.0, 0.0); glVertex2f(-0.5f, 0.5f);
	glColor3f(0.0, 0.0, 1.0); glVertex2f(0.5f, 0.5f);
	glColor3f(1.0, 1.0, 1.0); glVertex2f(0.5f, -0.5f);
	glEnd();

	glPopMatrix();

	//! Отключаем шейдерную программу 
	glUseProgram(0);

	glPushMatrix();
	//! Устанавливаем шейдерную программу текущей
	glUseProgram(program2);
	//////! Передаем юниформ в шейдер
	glUniform4f(ucolor1, options[1]['r'] / 255.f, options[1]['g'] / 255.f, options[1]['b'] / 255.f, 1.f);
	glUniform4f(ucolor2, options[2]['r'] / 255.f, options[2]['g'] / 255.f, options[2]['b'] / 255.f, 1.f);
	glUniform1i(uwidth, options[1]['w']);
	glUniform1i(udir, options[1]['d'] % 2);

	glTranslatef(0, .5f, 0);

	glBegin(GL_QUADS);
	glColor3f(1.0, 0.0, 0.0); glVertex2f(-0.5f, -0.5f);
	glColor3f(0.0, 1.0, 0.0); glVertex2f(-0.5f, 0.5f);
	glColor3f(0.0, 0.0, 1.0); glVertex2f(0.5f, 0.5f);
	glColor3f(1.0, 1.0, 1.0); glVertex2f(0.5f, -0.5f);
	glEnd();

	glPopMatrix();

	//! Отключаем шейдерную программу 
	glUseProgram(0);

	glFlush();

	checkOpenGLerror();

	glutSwapBuffers();
}
void specialKeys(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_PAGE_UP: rotate_z += 5; break;
	case GLUT_KEY_PAGE_DOWN: rotate_z -= 5; break;
	}
	glutPostRedisplay();
}

void keyFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '=':
		options[option_list][option_char] += speeds[option_list][option_char];
		break;
	case '-':
		options[option_list][option_char] -= speeds[option_list][option_char];
		break;
	case '0':
	case '1':
	case '2':
		option_list = key - '0';
		break;
	case 'r':
	case 'g':
	case 'b':
	case 'x':
	case 'y':
	case 'z':
	case 'd':
	case 'w':
		option_char = key;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Simple shaders");
	glClearColor(0, 0, 1, 0);
	setup_options();

	//! Обязательно перед инициализацией шейдеров  	
	GLenum glew_status = glewInit();
	if (GLEW_OK != glew_status)
	{
		//! GLEW не проинициализировалась 
		std::cout << "Error: " << glewGetErrorString(glew_status) << "\n";  	 	return 1;
	}

	//! Проверяем доступность OpenGL 2.0 
	if (!GLEW_VERSION_2_0)
	{
		//! OpenGl 2.0 оказалась не доступна  	 	
		std::cout << "No support for OpenGL 2.0 found\n";
		return 1;
	}

	//! Инициализация шейдеров 

	Program = initShader("shader01.cpp");
	ucolor = getUniform("color", Program);

	program2 = initShader("shader02.cpp");
	ucolor1 = getUniform("color1", program2);
	ucolor2 = getUniform("color2", program2);
	udir = getUniform("dir_x", program2);
	uwidth = getUniform("width", program2);

	glutReshapeFunc(resizeWindow);
	//glutIdleFunc(render2);
	glutDisplayFunc(render2);
	
	glutSpecialFunc(specialKeys);
	glutKeyboardFunc(keyFunc);
	glutMainLoop();

	//! Освобождение ресурсов 
	freeShader();
}
