#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "include/GLAUX.LIB")

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <math.h>
#include <iomanip>
#include <string>
#include "GL/freeglut.h"
#include "include/glaux.h"

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

static double square_y_scale = 0.05, square_size_raw = 300;
static double square_size = square_y_scale * square_size_raw;

static int w = 0, h = 0;
static double head_size = 10, tail_size = 20, car_up = 3;
static double car_center[]{ 0, (head_size + square_size) / 2 + car_up, 0 };
static double car_rotate = 0;
static double forward_speed = 100, side_speed = 30, car_forward_speed = 0, 
	car_side_speed = 0, 
	forward_speedup = 3, 
	side_speedup = 1.5,
	current_forward_speedup = 0,
	current_side_speedup = 0;

unsigned char yellow[]{ 255,255,0 }, white[]{ 255,255,255 };

double tail_center[]{ -(head_size + tail_size) / 2, (tail_size - head_size) / 2, 0 };

double dt = 0, old_t = 0, t = 0;

bool buffer[256];

static int lanternsCount = 3;
static bool lanternsOn = true;

AUX_RGBImageRec* square_img;
unsigned int square_tex;

AUX_RGBImageRec* metal_img;
unsigned int metal_tex;

AUX_RGBImageRec* lantern_img;
unsigned int lantern_tex;

void LoadAUXTextures() {
	square_img = auxDIBImageLoad("sources\\paving_stone_texture.bmp");
	glGenTextures(1, &square_tex);
	glBindTexture(GL_TEXTURE_2D, square_tex);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
		square_img->sizeX,
		square_img->sizeY,
		GL_RGB, GL_UNSIGNED_BYTE,
		square_img->data);

	metal_img = auxDIBImageLoad("sources\\metal.bmp");
	glGenTextures(1, &metal_tex);
	glBindTexture(GL_TEXTURE_2D, metal_tex);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
		metal_img->sizeX,
		metal_img->sizeY,
		GL_RGB, GL_UNSIGNED_BYTE,
		metal_img->data);

	lantern_img = auxDIBImageLoad("sources\\lantern.bmp");
	glGenTextures(1, &lantern_tex);
	glBindTexture(GL_TEXTURE_2D, lantern_tex);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
		lantern_img->sizeX,
		lantern_img->sizeY,
		GL_RGB, GL_UNSIGNED_BYTE,
		lantern_img->data);
}

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

void drawLantern(GLfloat x, GLfloat y, GLfloat z, GLenum light) {
	glPushMatrix();

	//переносим фонарь
	glTranslatef(x, y, z);

	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	//рисуем столб
	GLUquadricObj* quadricObj;
	quadricObj = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, metal_tex);
	gluQuadricOrientation(quadricObj, GLU_OUTSIDE);
	gluQuadricTexture(quadricObj, GL_TRUE);

	gluCylinder(quadricObj, 2.0f, 2.0f, 45.0f, 30, 30);

	gluDeleteQuadric(quadricObj);

	//задаем свечение лампы
	if (lanternsOn) {
		float emission[] = { 0.5, 0.5, 0.5, 1.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, emission);
	}
	
	glBindTexture(GL_TEXTURE_2D, lantern_tex);

	glTranslatef(0.0f, 0.0f, 45.0f);
	glScalef(1.0f, 1.0f, 1.3f);

	glColor3f(1.0f, 0.7f, 0.1f);

	//задаем источник света для лампы
	GLfloat pos[] = { 0.0f, 0.0f, 10.0f, 1.0f };
	glLightfv(light, GL_POSITION, pos);
	GLfloat diffuse[] = { 1.0f, 0.8f, 0.3f };
	glLightfv(light, GL_DIFFUSE, diffuse);

	drawCubeSimplified(2.5f);

	//зчищаем свечение текстуры
	if (lanternsOn) {
		float clear_emission[] = { 0.0, 0.0, 0.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, clear_emission);
	}

	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}

void drawSquare() {
	glPushMatrix();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

	//drawing a square with paving stone texture
	glScalef(1.0f, square_y_scale, 1.0f);
	glBindTexture(GL_TEXTURE_2D, square_tex);
	drawCubeSimplified(square_size_raw / 2);
	glPopMatrix();

	//drawing lanterns
	drawLantern(-50.f, 0.f, 43.f, GL_LIGHT1);
	drawLantern(-40.f, 0.f, -65.f, GL_LIGHT2);
	drawLantern(70.f, 0.f, 65.f, GL_LIGHT3);
}

double degToRad(double deg)
{
	const double halfC = M_PI / 180;
	return deg * halfC;
}

void drawCar()
{
	glPushMatrix();
	car_rotate += side_speed * dt * current_side_speedup * side_speedup;
	float yRad = degToRad(car_rotate);
	car_center[2] -= float(sin(yRad) * forward_speed * dt * current_forward_speedup  / forward_speedup);
	car_center[0] += float(cos(yRad) * forward_speed * dt * current_forward_speedup / forward_speedup);
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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	LoadAUXTextures();

	old_t = glutGet(GLUT_ELAPSED_TIME);
}

void KeyHandler()
{
	if (buffer['w'])
	{
		current_forward_speedup = std::fmin(current_forward_speedup + dt, forward_speedup);
	}
	else if (buffer['s'])
	{
		current_forward_speedup = std::fmax(current_forward_speedup - dt, -forward_speedup);
	}
	else
	{
		current_forward_speedup = sgn(current_forward_speedup) * std::fmax(0, sgn(current_forward_speedup) * current_forward_speedup - dt);
	}

	if (buffer['a'])
	{
		current_side_speedup = std::fmin(current_side_speedup + dt, side_speedup);
	}
	else if (buffer['d'])
	{
		current_side_speedup = std::fmax(current_side_speedup - dt, -side_speedup);
	}
	else
		current_side_speedup = sgn(current_side_speedup) * std::fmax(0, sgn(current_side_speedup) * current_side_speedup - dt);

}

void Update(void) {
	t = glutGet(GLUT_ELAPSED_TIME);
	dt = (t - old_t) / 1000.0;
	old_t = t;
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	KeyHandler();
	glLoadIdentity();

	//Задаем положение и вектор обзора
	gluLookAt(100.0f, 100.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	//Работаем с освещением
	if (lanternsOn) {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		glEnable(GL_LIGHT3);
	}
	else {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		glDisable(GL_LIGHT3);
	}

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


void keyboardDown(unsigned char key, int x, int y)
{
	buffer[key] = true;
	switch (key)
	{
	case 'l':
		lanternsOn = !lanternsOn;
		break;
	default:
		break;
	}
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