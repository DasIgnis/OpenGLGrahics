#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "include/GLAUX.LIB")

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "include/glaux.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"

int w, h;

GLuint Program;

GLuint VAO, VBO, EBO;

GLint Attrib_vertex;
GLint Unif_color;
GLint Unif_matr;
GLint Unif_MVP;

GLint Model_vertices_count;

bool loadModel(const char* path, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& verticesTexture, std::vector<glm::vec3>& normals) {
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
				vertIndices.push_back(_verticesInd[0]);
				vertIndices.push_back(_verticesInd[1]);
				vertIndices.push_back(_verticesInd[2]);

				textIndices.push_back(_texturesInd[0]);
				textIndices.push_back(_texturesInd[1]);
				textIndices.push_back(_texturesInd[2]);

				normalsIndices.push_back(_normalsInd[0]);
				normalsIndices.push_back(_normalsInd[1]);
				normalsIndices.push_back(_normalsInd[2]);
				break;
			case 6:
				vertIndices.push_back(_verticesInd[0]);
				vertIndices.push_back(_verticesInd[1]);
				vertIndices.push_back(_verticesInd[2]);

				textIndices.push_back(_texturesInd[0]);
				textIndices.push_back(_texturesInd[1]);
				textIndices.push_back(_texturesInd[2]);
				break;
			case 3:
				vertIndices.push_back(_verticesInd[0]);
				vertIndices.push_back(_verticesInd[1]);
				vertIndices.push_back(_verticesInd[2]);
			default:
				break;
			}
		}
	}

	for (int i = 0; i < vertIndices.size(); i++) {
		GLuint index = vertIndices[i];
		vertices.push_back(temp_vert[index - 1]);
	}

	for (int i = 0; i < textIndices.size(); i++) {
		GLuint index = textIndices[i];
		verticesTexture.push_back(temp_text[index - 1]);
	}

	for (int i = 0; i < normalsIndices.size(); i++) {
		GLuint index = normalsIndices[i];
		normals.push_back(temp_normals[index - 1]);
	}
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
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> textCoords;
	std::vector<glm::vec3> normals;
	bool res = loadModel("african_head.obj", vertices, textCoords, normals);
	Model_vertices_count = vertices.size();

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

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
		"uniform mat4 MVP;"
		"void main() {\n"
		"	gl_Position = MVP * vec4(coord, 1.0);\n"
		"}\n";

	const char* fsSource =
		"#version 330\n"
		"uniform vec4 color;\n"
		"void main() {\n"
		"	gl_FragColor = color;\n"
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

	const char* attr_name = "coord";
	Attrib_vertex = glGetAttribLocation(Program, attr_name);
	if (Attrib_vertex == -1) {
		std::cout << "could not bind uniform " << attr_name << std::endl;
		return;
	}

	const char* unif_name = "color";
	Unif_color = glGetUniformLocation(Program, unif_name);
	if (Unif_color == -1) {
		std::cout << "could not bind uniform " << unif_name << std::endl;
		return;
	};

	/*const char* unif_name2 = "matrix";
	Unif_matr = glGetUniformLocation(Program, unif_name2);
	if (Unif_matr == -1) {
		std::cout << "could not bind uniform " << unif_name2 << std::endl;
		return;
	};*/

	const char* unif_name3 = "MVP";
	Unif_MVP = glGetUniformLocation(Program, unif_name3);
	if (Unif_MVP == -1) {
		std::cout << "could not bind uniform " << unif_name3 << std::endl;
		return;
	};

	checkOpenGLerror();
}

void freeShader() {
	glUseProgram(0);
	glDeleteProgram(Program);
}

void render2() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	//gluLookAt(5, 5, 5, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glUseProgram(Program);
	static float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	// ! Передаем юниформ в шейдер
	glUniform4fv(Unif_color, 1, red);

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 200.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, 3),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;
	glUniformMatrix4fv(Unif_MVP, 1, GL_FALSE, &MVP[0][0]);


	glEnableVertexAttribArray(Attrib_vertex);
	// ! Подключаем VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// ! Отключаем VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// ! Передаем данные на видеокарту (рисуем)
	glDrawArrays(GL_TRIANGLES, 0, Model_vertices_count * sizeof(glm::vec3));
	// ! Отключаем массив атрибутов
	glDisableVertexAttribArray(Attrib_vertex);

	glUseProgram(0);

	glFlush();
	checkOpenGLerror();
	glutSwapBuffers();
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

	setupBuffers();
	initShader();

	glutReshapeFunc(resizeWindow);
	glutDisplayFunc(render2);

	glClearColor(0, 0, 0, 0);

	glutMainLoop();

	freeShader();
	freeVBO();

	return 0;
}