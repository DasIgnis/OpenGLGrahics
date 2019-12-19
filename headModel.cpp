#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "include/GLAUX.LIB")

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "packages/nupengl.core.0.1.0.1/build/native/include/GL/glew.h"
#include "packages/freeglut.2.8.1.15/build/native/include/GL/freeglut.h"
#include "include/glaux.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "shaders.h"

int w, h;

unsigned int MODE = 0;

float angle = 5.0f;
float angle_light = 0.0f;

GLuint Program;

GLuint VAO, VBO, EBO;
GLuint VAO_tex, VBO_tex, EBO_tex;

GLint Attrib_vertex;
GLint Attrib_texture;
GLint Attrib_normal;
GLint Unif_color;
GLint Unif_matr;
GLint Unif_MVP;
GLint Unif_Model;
GLint Unif_texture;

GLint Unif_point_transform;
GLint Unif_point_light;
GLint Unif_point_vertex;

GLint Unif_frag_light;
GLint Unif_frag_material;

GLint Model_vertices_count;
GLint Model_textures_count;

AUX_RGBImageRec* head_img;
GLuint head_tex;

void LoadAUXTextures() {
	head_img = auxDIBImageLoad("sources\\bravit.bmp");
	glGenTextures(1, &head_tex);
	glBindTexture(GL_TEXTURE_2D, head_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		head_img->sizeX,
		head_img->sizeY,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		head_img->data);
}

bool loadModel(const char* path,
	std::vector<GLfloat>& vertices) {
	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}

	std::vector<glm::vec3> temp_vert = std::vector<glm::vec3>();
	std::vector<glm::vec2> temp_text = std::vector<glm::vec2>();
	std::vector<glm::vec3> temp_normals = std::vector<glm::vec3>();

	std::vector<GLuint> vertIndices = std::vector<GLuint>();
	std::vector<GLuint> textIndices = std::vector<GLuint>();
	std::vector<GLuint> normalsIndices = std::vector<GLuint>();

	while (1) {

		char type[128];
		int res = fscanf(file, "%s", type);
		if (res == EOF)
			break;

		if (strcmp(type, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vert.push_back(vertex);
		}
		else if (strcmp(type, "vt") == 0) {
			glm::vec2 texture;
			fscanf(file, "%f %f\n", &texture.x, &texture.y);
			temp_text.push_back(texture);
		}
		else if (strcmp(type, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(type, "f") == 0) {
			GLuint _verticesInd[3], _texturesInd[3], _normalsInd[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&_verticesInd[0], &_texturesInd[0], &_normalsInd[0],
				&_verticesInd[1], &_texturesInd[1], &_normalsInd[1],
				&_verticesInd[2], &_texturesInd[2], &_normalsInd[2]);
			if (matches != 9) {
				matches = fscanf(file, "%d/%d %d/%d %d/%d\n",
					&_verticesInd[0], &_texturesInd[0],
					&_verticesInd[1], &_texturesInd[1],
					&_verticesInd[2], &_texturesInd[2]);
				if (matches != 6) {
					matches = fscanf(file, "%d %d %d\n", &_verticesInd[0], &_verticesInd[1], &_verticesInd[2]);
					if (matches != 3) {
						printf("Format error !\n");
						return false;
					}
				}
			}
			switch (matches)
			{
			case 9:
				vertIndices.push_back(_verticesInd[0] - 1);
				vertIndices.push_back(_verticesInd[1] - 1);
				vertIndices.push_back(_verticesInd[2] - 1);

				textIndices.push_back(_texturesInd[0] - 1);
				textIndices.push_back(_texturesInd[1] - 1);
				textIndices.push_back(_texturesInd[2] - 1);

				normalsIndices.push_back(_normalsInd[0] - 1);
				normalsIndices.push_back(_normalsInd[1] - 1);
				normalsIndices.push_back(_normalsInd[2] - 1);
				break;
			case 6:
				vertIndices.push_back(_verticesInd[0] - 1);
				vertIndices.push_back(_verticesInd[1] - 1);
				vertIndices.push_back(_verticesInd[2] - 1);

				textIndices.push_back(_texturesInd[0] - 1);
				textIndices.push_back(_texturesInd[1] - 1);
				textIndices.push_back(_texturesInd[2] - 1);
				break;
			case 3:
				vertIndices.push_back(_verticesInd[0] - 1);
				vertIndices.push_back(_verticesInd[1] - 1);
				vertIndices.push_back(_verticesInd[2] - 1);
			default:
				break;
			}
		}
	}

	std::vector<GLfloat> result = std::vector<GLfloat>();
	for (int i = 0; i < vertIndices.size(); i++) {
		int vertInd = vertIndices[i];
		vertInd = abs(vertInd);
		result.push_back(temp_vert[vertInd].x);
		result.push_back(temp_vert[vertInd].y);
		result.push_back(temp_vert[vertInd].z);
		if (textIndices.size() == vertIndices.size()) {
			int textInd = textIndices[i];
			textInd = abs(textInd);
			result.push_back(temp_text[textInd].x);
			result.push_back(temp_text[textInd].y);

			if (normalsIndices.size() == vertIndices.size()) {
				int normalInd = normalsIndices[i];
				normalInd = abs(normalInd);
				result.push_back(temp_normals[normalInd].x);
				result.push_back(temp_normals[normalInd].y);
				result.push_back(temp_normals[normalInd].z);
			}
		}
	}

	vertices = result;
}

bool saveModel(const char* path,
	std::vector<GLfloat> vertices) {
	return true;
}

void resizeWindow(int width, int height) {
	w = width;
	h = height;

	glViewport(0, 0, w, h);
}

void checkOpenGLerror() {
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL error!" << gluErrorString(errCode);
	}
}

void setupBuffers() {
	std::vector<GLfloat> vertices;
	bool res = loadModel("sources\\african_head.obj", vertices);
	Model_vertices_count = vertices.size();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	std::vector<GLuint> ebo_data = std::vector<GLuint>();
	for (int i = 0; i < Model_vertices_count; i++) {
		ebo_data.push_back(i);
	}
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_data.size() * sizeof(GLuint), &ebo_data[0], GL_STATIC_DRAW);

	checkOpenGLerror();
}

void initBuffers() {
	std::vector<GLfloat> vertices = { 0.0, 0.0, 0.0, 0, 0, 0, 0.2f, 0.3f, 0.6f,
									0.0, 0.0, 0.5, 0, 0, 0, 0.3f, 0.4f, 0.5f,
									0.0, 0.5, 0.1, 0, 0, 0, 0.8f, 0.5f, 0.3f,
									0.0, -0.5, 0.1, 0, 0, 0, 0.4f, 0.7f, 0.9f,
									0.0, 0.0, 0.1, 0, 0, 0, 0.7f, 0.6f, 0.2f,
									0.0, 0.0, 0.15, 0, 0, 0, 0.76f, 0.62f, 0.42f,
									0.0, 0.0, 0.2, 0, 0, 0, 0.67f, 0.26f, 0.92f,
									0.0, 0.0, 0.25, 0, 0, 0, 0.19f, 0.44f, 0.28f,
									0.0, 0.0, 0.3, 0, 0, 0, 0.54f, 0.14f, 0.52f,
									0.0, 0.0, 0.35, 0, 0, 0, 0.7f, 0.2f, 0.9f,
									0.0, 0.0, 0.4, 0, 0, 0, 0.51f, 0.51f, 0.23f,
									0.0, 0.0, 0.45, 0, 0, 0, 0.63f, 0.32f, 0.12f,
	};

	Model_vertices_count = vertices.size();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	std::vector<GLuint> ebo_data = {
		1, 2, 3
	};
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_data.size() * sizeof(GLuint), &ebo_data[0], GL_STATIC_DRAW);

	checkOpenGLerror();
}

void freeVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);
}

void initShader() {

	const char* vsSource =
		"#version 330\n"
		"attribute vec3 coord;\n"
		"attribute vec2 textureCoord;\n"
		"attribute vec3 normal;\n"
		"out vec4 color;\n"
		"void main() {\n"
		" gl_Position = vec4(coord, 1.0);\n"
		" color = vec4(normal, 1.0);\n"
		"}\n";

	const char* fsSource =
		"#version 330\n"
		"out vec4 FragColor;\n"
		"in vec4 color;\n"
		"void main() {\n"
		" FragColor = color;\n"
		"}\n";

	GLuint fShader;
	GLuint vShader;

	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	vShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(fShader, 1, &fsSource, NULL);
	glCompileShader(fShader);

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

	const char* attr_name = "coord";
	Attrib_vertex = glGetAttribLocation(Program, attr_name);
	if (Attrib_vertex == -1) {
		std::cout << "could not bind uniform " << attr_name << std::endl;
		return;
	}

	const char* attr_name3 = "normal";
	Attrib_normal = glGetAttribLocation(Program, attr_name3);
	if (Attrib_normal == -1) {
		std::cout << "could not bind uniform " << attr_name3 << std::endl;
		return;
	}

	checkOpenGLerror();
}


void freeShader() {
	glUseProgram(0);
	glDeleteProgram(Program);
}

void render2() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glLoadIdentity();

	glUseProgram(Program);

	float a = angle * 3.14f / 180.0f;

	glm::mat4 rotateY = { glm::cos(a),0.0f, glm::sin(a), 0.0f,
								0.0f, 1.0f, 0.0f, 0.0f,
								-glm::sin(a), 0.0f, glm::cos(a), 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f };

	glm::mat4 projection = glm::perspective(
		glm::radians(60.0f),
		4.0f / 3.0f,
		0.1f,
		100.0f
	);
	glm::mat4 view = glm::lookAt(
		glm::vec3(5, 0, 0),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);
	glm::mat4 viewProj = projection * view;

	float al = angle_light * 3.14f / 180.0f;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glDrawElements(GL_TRIANGLES, Model_vertices_count, GL_UNSIGNED_INT, 0);
	// ! Отключаем массив атрибутов
	//glDisableVertexAttribArray(Attrib_vertex);
	//glDisableVertexAttribArray(Attrib_texture);

	glUseProgram(0);

	glFlush();
	checkOpenGLerror();
	glutSwapBuffers();
}

void keySpecialFunc(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_F1:
		MODE = 0;
		break;
	case GLUT_KEY_F2:
		MODE = 1;
		break;
	case GLUT_KEY_F3:
		MODE = 2;
		break;
	case GLUT_KEY_F4:
		MODE = 3;
		break;
	default:
		break;
	}
	initShader();
	glutPostRedisplay();
}

void keyboardDown(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'a':
		angle += 1.0;
		break;
	case 'd':
		angle -= 1.0;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {

	//setlocale(LC_ALL, "RUSSIAN");

	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutCreateWindow("OpenGL");



	GLenum glew_status = glewInit();
	if (GLEW_OK != glew_status) {
		std::cout << "Error" << glewGetErrorString(glew_status) << "\n";
		return 1;
	}

	if (!GLEW_VERSION_2_0) {
		std::cout << "No supp for OPGL2.0\n";
		return 1;
	}

	LoadAUXTextures();
	initBuffers();
	initShader();

	glutKeyboardFunc(keyboardDown);
	glutSpecialFunc(keySpecialFunc);

	glutReshapeFunc(resizeWindow);
	glutDisplayFunc(render2);

	glClearColor(0, 0, 0, 0);

	glutMainLoop();

	freeShader();
	freeVBO();

	return 0;
}