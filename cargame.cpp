#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "include/GLAUX.LIB")

#include <stdio.h>
#include <stdlib.h>
#include "GL/freeglut.h"
#include "include/glaux.h"


static int w = 0, h = 0;

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
	glScalef(1.0f, 0.1f, 1.0f);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, square_tex);
	drawCubeSimplified(75.0f);
}

void Init(void) {
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	square_img = auxDIBImageLoad("D:\\Users\\acer\\source\\repos\\OpenGLLab\\Debug\\paving_stone_texture.bmp");
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