#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "include/GLAUX.LIB")

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
#include "GL/glew.h"
#include "GL/freeglut.h"
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

std::vector<GLfloat> vertices;
std::vector<GLuint> ebo_data;
glm::mat4 rotateY;

struct PointLight
{
	glm::vec4  position;
	glm::vec4  ambient;
	glm::vec4  diffuse;
	glm::vec4  specular;
	glm::vec3  attenuation;
};

struct Transform
{
	glm::mat4 model;
	glm::mat4 viewProjection;
	glm::mat3 normal;
	glm::vec3 viewPosition;
};

struct Material {
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 emission;
	float shininess;
};

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
	std::vector<GLfloat> vertices,
	std::vector<GLuint> ebo_data,
	glm::mat4 transform) {
	std::ofstream myfile;
	myfile.open(path);
	for (int i = 0; i < vertices.size(); i++) {
		if (i % 3 == 0) {
			glm::vec4 coord = { vertices[i], vertices[i + 1], vertices[i + 2], 1.0 };
			glm::vec4 newCoord = coord * transform;
			myfile << "v  " << newCoord.x << " " << newCoord.y << " " << newCoord.z << std::endl;
		}
	}
	for (int i = 0; i < ebo_data.size(); i++) {
		if (i % 3 == 0) {
			myfile << "f  " << ebo_data[i] << " " << ebo_data[i + 1] << " " << ebo_data[i + 2] << std::endl;
		}
	}
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
	bool res = loadModel("sources\\african_head.obj", vertices);
	Model_vertices_count = vertices.size();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	ebo_data = std::vector<GLuint>();
	for (int i = 0; i < Model_vertices_count; i++) {
		ebo_data.push_back(i);
	}
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_data.size() * sizeof(GLuint), &ebo_data[0], GL_STATIC_DRAW);

	checkOpenGLerror();
}

void initBuffers() {
	vertices = {
				-0.25,-0.25,-0.25,
				-0.25,-0.25,0.25,
				-0.125,-0.125,0.5,
				-0.3415063509461097,0.091506350946109621, -0.25,
				-0.3415063509461097,0.091506350946109621, 0.25,
				-0.17075317547305485,0.045753175473054811,0.5,
				-0.091506350946109746,0.34150635094610965,-0.25,
				-0.091506350946109746,0.34150635094610965,0.25,
				-0.045753175473054873,0.17075317547305483,0.5,
				0.24999999999999992,0.25000000000000011,-0.25,
				0.24999999999999992,0.25000000000000011,0.25,
				0.12499999999999996,0.12500000000000006,0.5,
				0.3415063509461097,-0.091506350946109483,-0.25,
				0.3415063509461097,-0.091506350946109483,0.25,
				0.17075317547305485,-0.045753175473054741,0.5,
				0.091506350946109871,-0.34150635094610959,-0.25,
				0.091506350946109871,-0.34150635094610959,-0.25,
				0.045753175473054936,-0.1707531754730548,0.5
	};

	Model_vertices_count = vertices.size() / 8;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	ebo_data = {
		3,0,1,
		3,1,4,

		4,1,2,
		4,2,5,

		6,3,4,
		6,4,7,

		7,4,5,
		7,5,8,

		9,6,7,
		9,7,10,

		10,7,8,
		10,8,11,

		12,9,10,
		12,10,13,

		13,10,11,
		13,11,14,

		15,12,13,
		15,13,16,

		16,13,14,
		16,14,17,

		0,15,16,
		0,16,1,

		1,16,17,
		1,17,2


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

	GLuint fShader;
	GLuint vShader;

	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	vShader = glCreateShader(GL_VERTEX_SHADER);

	if (MODE == 0 || MODE == 1) {
		if (MODE == 0) {
			glShaderSource(fShader, 1, &fsSourceLightingPhong, NULL);
			glCompileShader(fShader);
		}

		if (MODE == 1) {
			glShaderSource(fShader, 1, &fsSourceLightingPhongColor, NULL);
			glCompileShader(fShader);
		}

		glShaderSource(vShader, 1, &vsSourceLightingPhong, NULL);
		glCompileShader(vShader);
	}
	else if (MODE == 2 || MODE == 3) {
		if (MODE == 2) {
			glShaderSource(fShader, 1, &fsSourceLightingPhongInt, NULL);
			glCompileShader(fShader);
		}

		if (MODE == 3) {
			glShaderSource(fShader, 1, &fsSourceLightingPhongIntColor, NULL);
			glCompileShader(fShader);
		}

		glShaderSource(vShader, 1, &vsSourceLightingPhongInt, NULL);
		glCompileShader(vShader);
	}
	else {
		glShaderSource(fShader, 1, &fsSourceDiffColors, NULL);
		glCompileShader(fShader);

		glShaderSource(vShader, 1, &vsSourceDiffColors, NULL);
		glCompileShader(vShader);

	}

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

	const char* attr_name = "position";
	Attrib_vertex = glGetAttribLocation(Program, attr_name);
	if (Attrib_vertex == -1) {
		std::cout << "could not bind uniform " << attr_name << std::endl;
		return;
	}

	const char* attr_name2 = "textureCoord";
	Attrib_texture = glGetAttribLocation(Program, attr_name2);
	if (Attrib_texture == -1) {
		std::cout << "could not bind uniform " << attr_name2 << std::endl;
	}

	const char* attr_name3 = "normal";
	Attrib_normal = glGetAttribLocation(Program, attr_name3);
	if (Attrib_normal == -1) {
		std::cout << "could not bind uniform " << attr_name3 << std::endl;
	}

	if (MODE == 0 || MODE == 2) {
		const char* unif_name_t = "texture1";
		Unif_texture = glGetUniformLocation(Program, unif_name_t);
		if (Unif_texture == -1) {
			std::cout << "could not bind uniform " << unif_name_t << std::endl;
			return;
		};
	}

	if (MODE == 1 || MODE == 3 || MODE == 4) {
		const char* unif_name_color = "color";
		Unif_color = glGetUniformLocation(Program, unif_name_color);
		if (Unif_color == -1) {
			std::cout << "could not bind uniform" << unif_name_color << std::endl;
		}
	}

	checkOpenGLerror();
}

void PointLightSetup(GLuint program, const PointLight& light)
{
	glUniform4fv(glGetUniformLocation(program, "light.position"), 1, &light.position[0]);
	glUniform4fv(glGetUniformLocation(program, "light.ambient"), 1, &light.ambient[0]);
	glUniform4fv(glGetUniformLocation(program, "light.diffuse"), 1, &light.diffuse[0]);
	glUniform4fv(glGetUniformLocation(program, "light.specular"), 1, &light.specular[0]);
	glUniform3fv(glGetUniformLocation(program, "light.attenuation"), 1, &light.attenuation[0]);
}

void TransformSetup(GLuint program, const Transform& transform)
{
	glUniformMatrix4fv(glGetUniformLocation(program, "transform.model"), 1, GL_FALSE, &transform.model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "transform.viewProjection"), 1, GL_FALSE, &transform.viewProjection[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(program, "transform.normal"), 1, GL_FALSE, &transform.normal[0][0]);
	glUniform3fv(glGetUniformLocation(program, "transform.viewPosition"), 1, &transform.viewPosition[0]);
}

void MaterialSetup(GLuint program, const Material& material)
{
	glUniform4fv(glGetUniformLocation(program, "material.ambient"), 1, &material.ambient[0]);
	glUniform4fv(glGetUniformLocation(program, "material.diffuse"), 1, &material.diffuse[0]);
	glUniform4fv(glGetUniformLocation(program, "material.specular"), 1, &material.specular[0]);
	glUniform4fv(glGetUniformLocation(program, "material.emission"), 1, &material.emission[0]);
	glUniform1f(glGetUniformLocation(program, "material.shiness"), material.shininess);
}

void freeShader() {
	glUseProgram(0);
	glDeleteProgram(Program);
}

void render2() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glLoadIdentity();

	//gluLookAt(5, 5, 5, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glUseProgram(Program);
	static float red[4] = { 0.1f, 0.3f, 0.8f, 1.0f };
	// ! Передаем юниформ в шейдер
	glUniform4fv(Unif_color, 1, red);


	float a = angle * 3.14f / 180.0f;

	rotateY = { glm::cos(a),0.0f, glm::sin(a), 0.0f,
								0.0f, 1.0f, 0.0f, 0.0f,
								-glm::sin(a), 0.0f, glm::cos(a), 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f };

	if (MODE == 4) {
		glUniformMatrix4fv(glGetUniformLocation(Program, "model"), 1, GL_FALSE, &rotateY[0][0]);
	}

	Transform transform = Transform();
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

	if (MODE != 4) {
		transform.viewProjection = viewProj;
		transform.model = rotateY;
		transform.viewPosition = { 5.0f, 0.0f, 0.0f };
		transform.normal = glm::mat3(rotateY);

		TransformSetup(Program, transform);

		float al = angle_light * 3.14f / 180.0f;

		PointLight light = PointLight();
		light.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
		light.diffuse = { 0.9f, 0.9f, 0.9f, 1.0f };
		light.specular = { 0.0f, 0.0f, 0.0f, 1.0f };
		light.attenuation = { 0.7f, 0.1f, 0.0f };
		light.position = { 7.0f * sin(al), 0.0f, 7.0f * cos(al), 1.0f };

		PointLightSetup(Program, light);

		Material material = Material();
		material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
		material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
		material.emission = { 0.0f, 0.0f, 0.0f, 0.0f };
		material.specular = { 0.1f, 0.1f, 0.1f, 1.0f };
		material.shininess = 0.0f;

		MaterialSetup(Program, material);
	}

	if (MODE != 4) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
	}
	else {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
	}

	if (MODE != 4) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, head_tex);
		glUniform1i(Unif_texture, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}


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
	case GLUT_KEY_F5:
		MODE = 4;
		initBuffers();
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
	case 'q':
		angle_light += 1.0;
		break;
	case 'e':
		angle_light -= 1.0;
		break;
	case 'z':
		saveModel("example.obj", vertices, ebo_data, rotateY);
		break;
	case 'x':
		loadModel("example.obj", vertices);
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
	setupBuffers();
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