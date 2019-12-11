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
static double head_size = 10, tail_size = 20, wheel_size = 6, tire_size = 1.5, wheel_slices = 10, wheel_loops = 10,
	headlamp_size = 4, headlamp_stacks = 10, headlamp_slices = 10;
static double car_up = wheel_size / 2 + tire_size;
static double car_center[]{ 0, (head_size + square_size) / 2 + car_up, 0 };
static double car_rotate = 0;
static double forward_speed = 100, side_speed = 60,
	forward_speedup = 3, 
	side_speedup = 1.5,
	current_forward_speedup = 0,
	current_side_speedup = 0;

double tail_center[]{ -(head_size + tail_size) / 2, (tail_size - head_size) / 2, 0 };
double wheel_center[]{ (tail_size) / 2, -tail_size / 2, (tail_size + 1) / 2};
double headlamp_center[]{ head_size / 2, -(head_size - headlamp_size) / 2, (head_size - headlamp_size) / 2 };

double dt = 0, old_t = 0, t = 0;

bool buffer[256];

static int lanternsCount = 3;
static bool lanternsOn = true;
static bool carLightingOn = true;

AUX_RGBImageRec* square_img;
unsigned int square_tex;

AUX_RGBImageRec* metal_img;
unsigned int metal_tex;

AUX_RGBImageRec* lantern_img;
unsigned int lantern_tex;

AUX_RGBImageRec* cola_img;
unsigned int cola_tex;

AUX_RGBImageRec* wheels_img;
unsigned int wheels_tex;

AUX_RGBImageRec* gift_img;
unsigned int gift_tex;

AUX_RGBImageRec* truck_img;
unsigned int truck_tex;


GLdouble scale = 150.0;
GLdouble angle = 0.0;
GLdouble camX = scale * sin(angle);
GLdouble camZ = scale * cos(angle);

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

	cola_img = auxDIBImageLoad("sources\\cola.bmp");
	glGenTextures(1, &cola_tex);
	glBindTexture(GL_TEXTURE_2D, cola_tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
		cola_img->sizeX,
		cola_img->sizeY,
		GL_RGB, GL_BYTE,
		cola_img->data);

	wheels_img = auxDIBImageLoad("sources\\wheels.bmp");
	glGenTextures(1, &wheels_tex);
	glBindTexture(GL_TEXTURE_2D, wheels_tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
		wheels_img->sizeX,
		wheels_img->sizeY,
		GL_RGB, GL_BYTE,
		wheels_img->data);

	gift_img = auxDIBImageLoad("sources\\gift.bmp");
	glGenTextures(1, &gift_tex);
	glBindTexture(GL_TEXTURE_2D, gift_tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
		gift_img->sizeX,
		gift_img->sizeY,
		GL_RGB, GL_BYTE,
		gift_img->data);

	truck_img = auxDIBImageLoad("sources\\truck.bmp");
	glGenTextures(1, &truck_tex);
	glBindTexture(GL_TEXTURE_2D, truck_tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
		truck_img->sizeX,
		truck_img->sizeY,
		GL_RGB, GL_BYTE,
		truck_img->data);
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

/*
r = torus ring radius
c = torus tube radius
rSeg, cSeg = number of segments/detail
*/
void drawTorus(double r = 0.07, double c = 0.15,
	int rSeg = 16, int cSeg = 8,
	int texture = 0)
{
	glFrontFace(GL_CW);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	const double PI = M_PI;
	const double TAU = 2 * PI;

	for (int i = 0; i < rSeg; i++) {
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= cSeg; j++) {
			for (int k = 0; k <= 1; k++) {
				double s = (i + k) % rSeg + 0.5;
				double t = j % (cSeg + 1);

				double x = (c + r * cos(s * TAU / rSeg)) * cos(t * TAU / cSeg);
				double y = (c + r * cos(s * TAU / rSeg)) * sin(t * TAU / cSeg);
				double z = r * sin(s * TAU / rSeg);

				double u = (i + k) / (float)rSeg;
				double v = t / (float)cSeg;

				glTexCoord2d(u, v);
				glNormal3f(2 * x, 2 * y, 2 * z);
				glVertex3d(2 * x, 2 * y, 2 * z);
			}
		}
		glEnd();
	}

	glFrontFace(GL_CCW);
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
		float emission[] = { 0.3, 0.3, 0.3, 1.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, emission);
	}
	
	glBindTexture(GL_TEXTURE_2D, lantern_tex);

	glTranslatef(0.0f, 0.0f, 45.0f);
	glScalef(1.0f, 1.0f, 1.3f);

	//glColor3f(1.0f, 0.7f, 0.1f);

	//задаем источник света для лампы
	GLfloat pos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightfv(light, GL_POSITION, pos);
	GLfloat diffuse[] = { 1.0f, 0.8f, 0.3f };
	glLightfv(light, GL_DIFFUSE, diffuse);
	glLightf(light, GL_LINEAR_ATTENUATION, 0.005);

	drawCubeSimplified(2.5f);

	//зчищаем свечение текстуры
	if (lanternsOn) {
		float clear_emission[] = { 0.0, 0.0, 0.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, clear_emission);
	}

	//glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}

void drawSquare() {
	glPushMatrix();

	//drawing a square with paving stone texture
	glScalef(1.0f, square_y_scale, 1.0f);
	glBindTexture(GL_TEXTURE_2D, square_tex);
	drawCubeSimplified(square_size_raw / 2);
	glPopMatrix();

	//drawing lanterns
	drawLantern(-80.f, 0.f, 73.f, GL_LIGHT1);
	drawLantern(-70.f, 0.f, -95.f, GL_LIGHT2);
	drawLantern(100.f, 0.f, 65.f, GL_LIGHT3);
	drawLantern(90.f, 0.f, -100.f, GL_LIGHT4);

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, gift_tex);

	glPushMatrix();
	glTranslatef(-50.f, 18.f, 60.f);
	glRotatef(45.f, 0.f, 1.f, 0.f);
	drawCubeSimplified(8.f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(80.f, 15.f, 40.f);
	glRotatef(15.f, 0.f, 1.f, 0.f);
	glRotatef(90.f, 1.f, 0.f, 0.f);
	drawCubeSimplified(5.f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(105.f, 17.5f, -90.f);
	drawCubeSimplified(7.f);
	glPopMatrix();

	glPopMatrix();
}

double degToRad(double deg)
{
	const double halfC = M_PI / 180;
	return deg * halfC;
}


void setupCamLight(GLenum light) {
	GLfloat pos[] = { camX, scale, camZ };
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f };
	glLightfv(light, GL_DIFFUSE, diffuse);
	GLfloat spot_dir[] = { 0.f, 0.f, 0.f };
	glLightfv(light, GL_SPOT_DIRECTION, spot_dir);

	
	glLightf(light, GL_SPOT_EXPONENT, 5.0);
	glLightf(light, GL_LINEAR_ATTENUATION, 0.01);
}

void setupCarLight(GLenum light) {
	GLfloat pos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightfv(light, GL_POSITION, pos);
	GLfloat diffuse[] = { 1.0f, 0.8f, 0.8f };
	glLightfv(light, GL_DIFFUSE, diffuse);
	GLfloat spot_dir[] = {1.f, -0.3f, 0.f };
	glLightfv(light, GL_SPOT_DIRECTION, spot_dir);
	glLightf(light, GL_SPOT_CUTOFF, 30.0);
	glLightf(light, GL_SPOT_EXPONENT, 10.0);
	glLightf(light, GL_LINEAR_ATTENUATION, 0.01);
}

void drawCar()
{
	glPushMatrix();
	car_rotate += side_speed * dt * current_side_speedup / side_speedup;
	float yRad = degToRad(car_rotate);
	car_center[2] -= float(sin(yRad) * forward_speed * dt * current_forward_speedup  / forward_speedup);
	car_center[0] += float(cos(yRad) * forward_speed * dt * current_forward_speedup / forward_speedup);
	glTranslatef(car_center[0], car_center[1], car_center[2]);
	glRotatef(car_rotate, 0, 1, 0);
	// texture for head of car
	glBindTexture(GL_TEXTURE_2D, truck_tex);
	drawCubeSimplified(head_size / 2);

	glPushMatrix();
	glTranslatef(tail_center[0], tail_center[1], tail_center[2]);
	// texture for tail of car
	glBindTexture(GL_TEXTURE_2D, cola_tex);
	drawCubeSimplified(tail_size / 2);

	// texture for wheels
	int wheel_texture = wheels_tex;

	glPushMatrix();
	glTranslatef(wheel_center[0], wheel_center[1], wheel_center[2]);
	glRotatef(side_speed * current_side_speedup / side_speedup, 0, 1, 0);
	drawTorus(tire_size / 2, wheel_size / 2 - tire_size, wheel_slices, wheel_loops, wheel_texture);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(wheel_center[0], wheel_center[1], -wheel_center[2]);
	glRotatef(side_speed * current_side_speedup / side_speedup, 0, 1, 0);

	drawTorus(tire_size / 2, wheel_size / 2 - tire_size, wheel_slices, wheel_loops, wheel_texture);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-wheel_center[0], wheel_center[1], wheel_center[2]);
	drawTorus(tire_size / 2, wheel_size / 2 - tire_size, wheel_slices, wheel_loops, wheel_texture);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-wheel_center[0], wheel_center[1], -wheel_center[2]);
	drawTorus(tire_size / 2, wheel_size / 2 - tire_size, wheel_slices, wheel_loops, wheel_texture);
	glPopMatrix();

	glPopMatrix();

	GLUquadricObj* quadricObj;
	quadricObj = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, lantern_tex);
	gluQuadricOrientation(quadricObj, GLU_OUTSIDE);
	gluQuadricTexture(quadricObj, GL_TRUE);

	//Одна фара
	glPushMatrix();

	float emission[] = { 0.5, 0.5, 0.5, 1.0 };
	float clear_emission[] = { 0.0, 0.0, 0.0, 1.0 };
	glTranslatef(headlamp_center[0], headlamp_center[1], -headlamp_center[2]);

	if (carLightingOn) {
		glMaterialfv(GL_FRONT, GL_EMISSION, emission);
	}
	
	setupCarLight(GL_LIGHT5);
	gluSphere(quadricObj, headlamp_size / 2, headlamp_slices, headlamp_stacks);

	if (carLightingOn) {
		glMaterialfv(GL_FRONT, GL_EMISSION, clear_emission);
	}

	glPopMatrix();

	//Вторая фара
	glPushMatrix();
	glTranslatef(headlamp_center[0], headlamp_center[1], headlamp_center[2]);

	if (carLightingOn) {
		glMaterialfv(GL_FRONT, GL_EMISSION, emission);
	}

	setupCarLight(GL_LIGHT6);
	gluSphere(quadricObj, headlamp_size / 2, headlamp_slices, headlamp_stacks);
	if (carLightingOn) {
		glMaterialfv(GL_FRONT, GL_EMISSION, clear_emission);
	}

	glPopMatrix();

	

	glPopMatrix();
}

void Init(void) {
	glClearColor(0.0f, 0.0f, .0f, .0f);
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

	 camX = scale * sin(angle);
	 camZ = scale * cos(angle);
	//Задаем положение и вектор обзора

	 setupCamLight(GL_LIGHT0);
	 glEnable(GL_LIGHT0);

	gluLookAt(camX, scale, camZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	

	//Работаем с освещением
	if (lanternsOn) {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		glEnable(GL_LIGHT3);
		glEnable(GL_LIGHT4);
	}
	else {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		glDisable(GL_LIGHT3);
		glDisable(GL_LIGHT4);
	}
	if (carLightingOn) {
		glEnable(GL_LIGHT5);
		glEnable(GL_LIGHT6);
	}
	else {
		glDisable(GL_LIGHT5);
		glDisable(GL_LIGHT6);
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
	case 'k':
		carLightingOn = !carLightingOn;
		break;
	case 'f':
		angle -= 0.1;
		break;
	case 'h':
		angle += 0.1;
		break;
	
	default:
		break;
	}
}

void keyboardUp(unsigned char key, int x, int y)
{
	buffer[key] = false;
}

void mouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0)
	{
		scale += 15;
	}
	else
	{
		scale -= 15;
	}

	return;
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
	glutMouseWheelFunc(mouseWheel);

	glutMainLoop();
	return 0;
}