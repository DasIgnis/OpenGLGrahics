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

float angle_horizontal = 0.0f;
float angle_vertical = 0.0f;
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

AUX_RGBImageRec* wood_img;
GLuint wood_tex;

AUX_RGBImageRec* chair_img;
GLuint chair_tex;

AUX_RGBImageRec* cup_img;
GLuint cup_tex;

std::vector<GLfloat> vertices_cup;
std::vector<GLfloat> vertices_chair;
std::vector<GLfloat> vertices_table;
std::vector<GLfloat> vertices_shar;
std::vector<GLuint> ebo_data;
glm::mat4 rotateY;

struct PointLight
{
	glm::vec4  position;
	glm::vec4  ambient;
	glm::vec4  diffuse;
	glm::vec4  specular;
	glm::vec3  attenuation;
	glm::vec4  lightColor;
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
	head_img = auxDIBImageLoad("sources\\metal.bmp");
	glGenTextures(1, &head_tex);
	glBindTexture(GL_TEXTURE_2D, head_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		head_img->sizeX,
		head_img->sizeY,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		head_img->data);

	wood_img = auxDIBImageLoad("sources\\wood.bmp");
	glGenTextures(1, &wood_tex);
	glBindTexture(GL_TEXTURE_2D, wood_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		wood_img->sizeX,
		wood_img->sizeY,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		wood_img->data);

	chair_img = auxDIBImageLoad("sources\\chair.bmp");
	glGenTextures(1, &chair_tex);
	glBindTexture(GL_TEXTURE_2D, chair_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		chair_img->sizeX,
		chair_img->sizeY,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		chair_img->data);

	cup_img = auxDIBImageLoad("sources\\cup.bmp");
	glGenTextures(1, &cup_tex);
	glBindTexture(GL_TEXTURE_2D, cup_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		cup_img->sizeX,
		cup_img->sizeY,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		cup_img->data);
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

void loadModels() {
	loadModel("sources\\cup.obj", vertices_cup);
	loadModel("sources\\chair.obj", vertices_chair);
	loadModel("sources\\shar.obj", vertices_shar);
	loadModel("sources\\table.obj", vertices_table);
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

void setupBuffers(std::vector<GLfloat> vertices) {
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

	if (MODE == 0) {
		glShaderSource(fShader, 1, &fsSourceLightingPhong, NULL);
		glCompileShader(fShader);

		glShaderSource(vShader, 1, &vsSourceLightingPhong, NULL);
		glCompileShader(vShader);
	}

	if (MODE == 1) {
		glShaderSource(fShader, 1, &fsSourceLightingLambert, NULL);
		glCompileShader(fShader);

		glShaderSource(vShader, 1, &vsSourceLightingLambert, NULL);
		glCompileShader(vShader);
	}

	if (MODE == 2) {
		glShaderSource(fShader, 1, &fsSourceLightingPhongDirection, NULL);
		glCompileShader(fShader);

		glShaderSource(vShader, 1, &vsSourceLightingPhongDirection, NULL);
		glCompileShader(vShader);
	}

	if (MODE == 3) {
		glShaderSource(fShader, 1, &fsSourceLightingBlinnPhong, NULL);
		glCompileShader(fShader);

		glShaderSource(vShader, 1, &vsSourceLightingPhong, NULL);
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

	const char* unif_name_t = "texture1";
	Unif_texture = glGetUniformLocation(Program, unif_name_t);
	if (Unif_texture == -1) {
		std::cout << "could not bind uniform " << unif_name_t << std::endl;
		return;
	};

	checkOpenGLerror();
}

void PointLightSetup(GLuint program, const PointLight& light)
{
	glUniform4fv(glGetUniformLocation(program, "light.position"), 1, &light.position[0]);
	glUniform4fv(glGetUniformLocation(program, "light.ambient"), 1, &light.ambient[0]);
	glUniform4fv(glGetUniformLocation(program, "light.diffuse"), 1, &light.diffuse[0]);
	glUniform4fv(glGetUniformLocation(program, "light.specular"), 1, &light.specular[0]);
	glUniform3fv(glGetUniformLocation(program, "light.attenuation"), 1, &light.attenuation[0]);
	glUniform4fv(glGetUniformLocation(program, "light.lightColor"), 1, &light.lightColor[0]);
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

void prepareTexture(GLuint tex) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(Unif_texture, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void render2() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glLoadIdentity();

	setupBuffers(vertices_cup);

	glUseProgram(Program);

	float ah = angle_horizontal * 3.14f / 180.0f;
	float av = angle_vertical * 3.14f / 180.0f;
	glm::vec3 cameraPosition = { 8.0f * sin(ah), 5.0f * sin(av), 8.0f * cos(ah) };

	Transform transform = Transform();
	glm::mat4 projection = glm::perspective(
		glm::radians(60.0f),
		4.0f / 3.0f,
		0.1f,
		100.0f
	);
	glm::mat4 view = glm::lookAt(
		cameraPosition,
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);
	glm::mat4 viewProj = projection * view;

	float a = 0 * 3.14f / 180.0f;
	//-------CUP SETUP------------
	glm::mat4 rotateY = { glm::cos(a),0.0f, glm::sin(a), 0.0f,
								0.0f, 1.0f, 0.0f, 0.0f,
								-glm::sin(a), 0.0f, glm::cos(a), 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f };

	glm::mat4 scale = { 0.2, 0.0, 0.0, 0.0,
							0.0, 0.2, 0.0, 0.0,
							0.0, 0.0, 0.2, 0.0,
							0.0, 0.0, 0.0, 1.0 };

	glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(4.f, 8.0f, -5.f));

	glm::mat4 transformMat = scale * rotateY * translate;

	transform.viewProjection = viewProj;
	transform.model = transformMat;
	transform.viewPosition = cameraPosition;
	transform.normal = glm::mat3(transformMat);

	TransformSetup(Program, transform);

	float al = angle_light * 3.14f / 180.0f;

	PointLight light = PointLight();
	light.ambient = { 0.8f, 0.8f, 0.8f, 1.0f };
	light.diffuse = { 0.9f, 0.9f, 0.9f, 1.0f };
	light.specular = { 0.6f, 0.6f, 0.6f, 1.0f };
	light.attenuation = { 0.7f, 0.1f, 0.0f };
	light.position = { 5.0f * sin(al), 4.0f, 5.0f * cos(al), 1.0f };
	light.lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	PointLightSetup(Program, light);

	Material material = Material();
	material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	material.emission = { 0.0f, 0.0f, 0.0f, 0.0f };
	material.specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	material.shininess = 16.0f;

	MaterialSetup(Program, material);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	prepareTexture(cup_tex);


	glDrawElements(GL_TRIANGLES, Model_vertices_count, GL_UNSIGNED_INT, 0);

	//------------CHAIR SETUP--------------
	setupBuffers(vertices_chair);

	transformMat = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 1.0f, 0.0f))
		* glm::scale(glm::mat4(1.f), glm::vec3(0.3f, 0.3f, 0.3f))
		* glm::rotate(glm::mat4(1.f), 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	transform.model = transformMat;
	transform.normal = glm::mat3(transformMat);

	TransformSetup(Program, transform);
	PointLightSetup(Program, light);

	material.specular = { 0.1f, 0.1f, 0.1f, 1.0f };
	material.shininess = 16.0f;
	MaterialSetup(Program, material);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	prepareTexture(chair_tex);


	glDrawElements(GL_TRIANGLES, Model_vertices_count, GL_UNSIGNED_INT, 0);


	//------TABLE SETUP-----
	setupBuffers(vertices_table);

	transformMat = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -0.5f, 0.0f))
		* glm::scale(glm::mat4(1.f), glm::vec3(4.f, 4.f, 4.f))
		* glm::rotate(glm::mat4(1.f), 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	transform.model = transformMat;
	transform.normal = glm::mat3(transformMat);

	TransformSetup(Program, transform);
	PointLightSetup(Program, light);
	MaterialSetup(Program, material);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	prepareTexture(wood_tex);

	glDrawElements(GL_TRIANGLES, Model_vertices_count, GL_UNSIGNED_INT, 0);


	//------------ SHAR SETUP---------------

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
		angle_horizontal += 2.0;
		break;
	case 'd':
		angle_horizontal -= 2.0;
		break;
	case 'w':
		angle_vertical += 2.0;
		break;
	case 's':
		angle_vertical -= 2.0;
		break;
	case 'q':
		angle_light += 2.0;
		break;
	case 'e':
		angle_light -= 2.0;
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
	loadModels();
	setupBuffers(vertices_cup);
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