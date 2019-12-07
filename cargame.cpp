#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "include/GLAUX.LIB")

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include<cstdlib>
#include<cmath>
#include <math.h>
#include<iomanip>
#include <string>
#include "GL/freeglut.h"
#include "include/glaux.h"

static double square_y_scale = 0.1, square_size_raw = 150;
static double square_size = square_y_scale * square_size_raw;

static int w = 0, h = 0;
static double head_size = 10, tail_size = 20, car_up = 3;
static double car_center[]{ 0, (head_size + square_size) / 2 + car_up, 0 };
static double car_rotate = 0;
static double forward_speed = 2, side_speed = 4, car_forward_speed = 0, car_side_speed = 0;
unsigned char yellow[]{ 255,255,0 }, white[]{ 255,255,255 };

double tail_center[]{ -(head_size + tail_size) /2, (tail_size - head_size)/2, 0 };

bool buffer[256];

AUX_RGBImageRec* square_img;
unsigned int square_tex;

void drawCubeSimplified(GLdouble size) {
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex3d(-size, size, -size);
	glTexCoord2d(1, 0); glVertex3d(size, size, -size);
	glTexCoord2d(1, 1); glVertex3d(size, size, size);
	glTexCoord2d(0, 1); glVertex3d(-size, size, size);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex3d(-size, -size, -size);
	glTexCoord2d(1, 0); glVertex3d(size, -size, -size);
	glTexCoord2d(1, 1); glVertex3d(size, -size, size);
	glTexCoord2d(0, 1); glVertex3d(-size, -size, size);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex3d(size, size, -size);
	glTexCoord2d(1, 0); glVertex3d(size, -size, -size);
	glTexCoord2d(1, 1); glVertex3d(size, -size, size);
	glTexCoord2d(0, 1); glVertex3d(size, size, size);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex3d(-size, size, -size);
	glTexCoord2d(1, 0); glVertex3d(-size, -size, -size);
	glTexCoord2d(1, 1); glVertex3d(-size, -size, size);
	glTexCoord2d(0, 1); glVertex3d(-size, size, size);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex3d(-size, size, size);
	glTexCoord2d(1, 0); glVertex3d(size, size, size);
	glTexCoord2d(1, 1); glVertex3d(size, -size, size);
	glTexCoord2d(0, 1); glVertex3d(-size, -size, size);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex3d(-size, size, -size);
	glTexCoord2d(1, 0); glVertex3d(size, size, -size);
	glTexCoord2d(1, 1); glVertex3d(size, -size, -size);
	glTexCoord2d(0, 1); glVertex3d(-size, -size, -size);
	glEnd();
}


void drawSquare() {
	glScalef(1.0f, square_y_scale, 1.0f);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, square_tex);
	drawCubeSimplified(square_size_raw / 2);
}

double degToRad(double deg)
{
	const double halfC = M_PI / 180;
	return deg * halfC;
}

void drawCar()
{
	glPushMatrix();
	car_rotate += car_side_speed;
	float yRad = degToRad(car_rotate);
	car_center[2] -= float(sin(yRad) * car_forward_speed);
	car_center[0] += float(cos(yRad) * car_forward_speed);
	glTranslatef(car_center[0], car_center[1], car_center[2]);
	glRotatef(car_rotate, 0, 1, 0);
	
	glColor3ubv(yellow);
	glutSolidCube(head_size);
	glPushMatrix();
	glTranslatef(tail_center[0], tail_center[1], tail_center[2]);
	glColor3ubv(white);
	glutSolidCube(tail_size);
	glPopMatrix();
	glPopMatrix();
}

void Init(void) {
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
}

void KeyHandler()
{
	if (buffer['w'])
	{
		car_forward_speed = forward_speed;
	}
	else if (buffer['s'])
	{
		car_forward_speed = -forward_speed;
	}
	else
		car_forward_speed = 0;

	if (buffer['a'])
	{
		car_side_speed = side_speed;
	}
	else if (buffer['d'])
	{
		car_side_speed = -side_speed;
	}
	else
		car_side_speed = 0;

	square_img = auxDIBImageLoad("sources\\paving_stone_texture.bmp");
	glGenTextures(1, &square_tex);
	glBindTexture(GL_TEXTURE_2D, square_tex);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
		square_img->sizeX,
		square_img->sizeY,
		GL_RGB, GL_UNSIGNED_BYTE,
		square_img->data);
}

void Update(void) {
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	KeyHandler();

	glLoadIdentity();	

	//Задаем положение и вектор обзора
	gluLookAt(100.0f, 100.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	drawSquare();

	drawCar();

	glFlush();
	glutSwapBuffers();
}

void Reshape(int width, int height) {
	w = width;
	h = height;

	glViewport(0, 0, w, h);
	//Перспективное преобразование
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//Задаем матрицу перспективного преобразования
	gluPerspective(65.0f, w / h, 1.0f, 1000.0f);
}

//void keyboardSpecialKeys(int key, int x, int y) {
//	switch (key)
//	{
//	case GLUT_KEY_LEFT:
//		rotate_y[id] += 5;
//		break;
//	case GLUT_KEY_RIGHT:
//		rotate_y[id] -= 5;
//		break;
//	case GLUT_KEY_UP:
//		rotate_x[id] += 5;
//		break;
//	case GLUT_KEY_DOWN:
//		rotate_x[id] -= 5;
//		break;
//	default:
//		break;
//	}
//	glutPostRedisplay();
//}


void keyboardDown(unsigned char key, int x, int y)
{
	buffer[key] = true;
}

void keyboardUp(unsigned char key, int x, int y)
{
	buffer[key] = false;
}

int main(int argc, char* argv[]) {

	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("OpenGL");

	glutIdleFunc(Update);
	glutDisplayFunc(Update);
	glutReshapeFunc(Reshape);

	Init();
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
	//glutSpecialFunc(keyboardSpecialKeys);
	glutMainLoop();
	return 0;
}