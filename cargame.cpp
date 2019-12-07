#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "include/GLAUX.LIB")

#include <stdio.h>
#include <stdlib.h>
#include "GL/freeglut.h"
#include "include/glaux.h"


static int w = 0, h = 0;

AUX_RGBImageRec* square_img;
unsigned int square_tex;

AUX_RGBImageRec* metal_img;
unsigned int metal_tex;

AUX_RGBImageRec* lantern_img;
unsigned int lantern_tex;

void LoadAUXTextures(){
	square_img = auxDIBImageLoad("D:\\Users\\acer\\source\\repos\\OpenGLLab\\Debug\\paving_stone_texture.bmp");
	glGenTextures(1, &square_tex);
	glBindTexture(GL_TEXTURE_2D, square_tex);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
		square_img->sizeX,
		square_img->sizeY,
		GL_RGB, GL_UNSIGNED_BYTE,
		square_img->data);

	metal_img = auxDIBImageLoad("D:\\Users\\acer\\source\\repos\\OpenGLLab\\Debug\\metal.bmp");
	glGenTextures(1, &metal_tex);
	glBindTexture(GL_TEXTURE_2D, metal_tex);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
		metal_img->sizeX,
		metal_img->sizeY,
		GL_RGB, GL_UNSIGNED_BYTE,
		metal_img->data);

	lantern_img = auxDIBImageLoad("D:\\Users\\acer\\source\\repos\\OpenGLLab\\Debug\\lantern.bmp");
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

void drawLantern() {
	glPushMatrix();
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	GLUquadricObj* quadricObj;
	quadricObj = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, metal_tex);
	gluQuadricOrientation(quadricObj, GLU_OUTSIDE);
	gluQuadricTexture(quadricObj, GL_TRUE);

	gluCylinder(quadricObj, 2.0f, 2.0f, 45.0f, 30, 30);

	//float emission[] = { 0.5, 0.5, 0.5, 1.0 };
	//glMaterialfv(GL_FRONT, GL_EMISSION, emission);

	GLfloat pos[] = { 1.0f,1.0f,1.0f,1.0f };
	glLightfv(GL_DIFFUSE, GL_POSITION, pos);

	glBindTexture(GL_TEXTURE_2D, lantern_tex);
	gluQuadricOrientation(quadricObj, GLU_OUTSIDE);
	gluQuadricTexture(quadricObj, GL_TRUE);

	glTranslatef(0.0f, 0.0f, 45.0f);

	gluCylinder(quadricObj, 3.0f, 3.0f, 5.0f, 30, 30);

	gluDeleteQuadric(quadricObj);

	glPopMatrix();
}

void drawSquare() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//drawing a square with paving stone texture
	glPushMatrix();
	glScalef(1.0f, 0.1f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, square_tex);
	drawCubeSimplified(75.0f);
	glPopMatrix();

	//drawing lanterns
	glPushMatrix();
	drawLantern();
	glPopMatrix();
}

void Init(void) {
	//glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	LoadAUXTextures();
}

void Update(void) {
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();	

	//Задаем положение и вектор обзора
	gluLookAt(100.0f, 100.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	drawSquare();

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
	glutMainLoop();
	return 0;
}