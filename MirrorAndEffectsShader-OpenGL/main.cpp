#pragma comment (lib, "Shell32")

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "shader.h"

#define INC_VAL     2.0f
#ifndef M_PI
#define M_PI        3.14159265359
#endif

glm::vec3 mirrorPosition = glm::vec3(0, 0, -5);

GLsizei g_width = 1280;
GLsizei g_height = 720;

GLboolean g_rotate = GL_TRUE;

GLfloat g_light0_pos[4] = { 10.0f, 5.0f, 0.0f, 1.0f };

static int shaderNumber = 1;
std::string textureName = "1.bmp";

GLdouble eqn1[4] = { 0.0, 0.0, 1.0, 0.0 };

GLfloat g_inc = 0.0f;
GLfloat angleCube = 0.0f;
GLuint ps, vs, prog, r_mod;
float angles = 0;

GLfloat positionUpDownChange = 0.0f;
GLfloat positionLeftRightChange = 0.0f;

const float MOVE_CONST = 0.1f;

bool enableRotation = false;

static unsigned int texture[2]; // indeksy tekstur.

struct BitMapFile
{
	int sizeX;
	int sizeY;
	unsigned char* data;
};

struct QuadFaceColor
{
	float r;
	float g;
	float b;

	QuadFaceColor(float _r, float _g, float _b)
	{
		r = _r;
		g = _g;
		b = _b;
	}
};

static void StartDrawingTexture()
{
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_2D);

}

static void StopDrawingTexture()
{
	glDisable(GL_TEXTURE_2D);
}

BitMapFile* getBMPData(std::string filename)
{
	BitMapFile* bmp = new BitMapFile;
	unsigned int size, offset, headerSize;

	std::ifstream infile(filename.c_str(), std::ios::binary);

	infile.seekg(10);
	infile.read((char*)&offset, 4);

	infile.read((char*)&headerSize, 4);

	infile.seekg(18);
	infile.read((char*)&bmp->sizeX, 4);
	infile.read((char*)&bmp->sizeY, 4);

	size = bmp->sizeX * bmp->sizeY * 24;
	bmp->data = new unsigned char[size];

	infile.seekg(offset);
	infile.read((char*)bmp->data, size);

	int temp;
	for (int i = 0; i < size; i += 3)
	{
		temp = bmp->data[i];
		bmp->data[i] = bmp->data[i + 2];
		bmp->data[i + 2] = temp;
	}

	return bmp;
}

void loadExternalTextures()
{
	BitMapFile* image[2];

	for (int i = 0; i < 2; i++)
	{
		image[i] = getBMPData(textureName);
		glBindTexture(GL_TEXTURE_2D, texture[i]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[i]->sizeX, image[i]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image[i]->data);
	}
}

void initialize()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(2.0f);

	glEnable(GL_LIGHTING);
	glLightModeli(GL_FRONT, GL_TRUE);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHT0);

	loadExternalTextures();
}

void drawQuadFace(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, QuadFaceColor color)
{
	glColor3f(color.r, color.g, color.b);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(p1.x, p1.y, p1.z);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(p2.x, p2.y, p2.z);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(p3.x, p3.y, p3.z);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(p4.x, p4.y, p4.z);
}

void drawCube(GLfloat centerX, GLfloat centerY, GLfloat centerZ, GLfloat size)
{
	StartDrawingTexture();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glm::vec3 p1 = glm::vec3(centerX + 1.0 * size, centerY + 1.0 * size, centerZ - 1.0 * size); // 0
	glm::vec3 p2 = glm::vec3(centerX - 1.0 * size, centerY + 1.0 * size, centerZ - 1.0 * size); // 1
	glm::vec3 p3 = glm::vec3(centerX - 1.0 * size, centerY + 1.0 * size, centerZ + 1.0 * size); // 2
	glm::vec3 p4 = glm::vec3(centerX + 1.0 * size, centerY + 1.0 * size, centerZ + 1.0 * size); // 3
	glm::vec3 p5 = glm::vec3(centerX + 1.0 * size, centerY - 1.0 * size, centerZ + 1.0 * size); // 4
	glm::vec3 p6 = glm::vec3(centerX - 1.0 * size, centerY - 1.0 * size, centerZ + 1.0 * size); // 5
	glm::vec3 p7 = glm::vec3(centerX - 1.0 * size, centerY - 1.0 * size, centerZ - 1.0 * size); // 6
	glm::vec3 p8 = glm::vec3(centerX + 1.0 * size, centerY - 1.0 * size, centerZ - 1.0 * size); // 7

	glBegin(GL_QUADS);

	// alternative way to draw a cube
	//GLfloat vertices[] = {	p1.x, p1.y, p1.z, 
	//						p2.x, p2.y, p2.z, 
	//						p3.x, p3.y, p3.z, 
	//						p4.x, p4.y, p4.z,
	//						p5.x, p5.y, p5.z, 
	//						p6.x, p6.y, p6.z,
	//						p7.x, p7.y, p7.z, 
	//						p8.x, p8.y, p8.z };

	//GLubyte indices[] = {	0, 1, 3, 3, 2, 1,
	//						1, 6, 2, 2, 5, 6,
	//						6, 1, 0, 0, 7, 6, 
	//						6, 5, 4, 4, 5, 2,
	//						2, 3, 4, 4, 7, 3,
	//						3, 0, 7, 7, 6, 4};

	//glEnableClientState(GL_VERTEX_ARRAY);
	//glVertexPointer(3, GL_FLOAT, 0, vertices);
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);
	//glDisableClientState(GL_VERTEX_ARRAY);

	drawQuadFace(p1, p2, p3, p4, QuadFaceColor(1.0, 1.0, 1.0));
	drawQuadFace(p5, p6, p7, p8, QuadFaceColor(1.0, 1.0, 1.0));
	drawQuadFace(p4, p3, p6, p5, QuadFaceColor(1.0, 1.0, 1.0));
	drawQuadFace(p8, p7, p2, p1, QuadFaceColor(1.0, 1.0, 1.0));
	drawQuadFace(p3, p2, p7, p6, QuadFaceColor(1.0, 1.0, 1.0));
	drawQuadFace(p1, p4, p5, p8, QuadFaceColor(1.0, 1.0, 1.0));

	glEnd();

	StopDrawingTexture();
}

void keyboardFunc(unsigned char key, int x, int y)
{
	switch (key) {
	case '1':
		shaderNumber = 1;
		break;
	case '2':
		shaderNumber = 2;
		break;
	case '3':
		shaderNumber = 3;
		break;
	case '4':
		shaderNumber = 4;
		break;
	case 'w':
		positionUpDownChange += MOVE_CONST;
		break;
	case 's':
		positionUpDownChange -= MOVE_CONST;
		break;
	case 'a':
		positionLeftRightChange -= MOVE_CONST;
		break;
	case 'd':
		positionLeftRightChange += MOVE_CONST;
		break;
	case 'z':
		textureName = "1.bmp";
		loadExternalTextures();
		break;
	case 'x':
		textureName = "2.bmp";
		loadExternalTextures();
		break;
	case 'c':
		textureName = "3.bmp";
		loadExternalTextures();
		break;
	case 'v':
		textureName = "4.bmp";
		loadExternalTextures();
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void drawMirrorReflection(int number)
{
	std::string vsPath = "shaders/vs";
	vsPath += number == 0 ? ".shader" : std::to_string(number) + ".shader";
	std::string fsPath = "shaders/fs";
	fsPath += number == 0 ? ".shader" : std::to_string(number) + ".shader";
	Shader shader = Shader(vsPath.c_str(), fsPath.c_str());
	shader.use();

	angleCube -= 0.15f;

	float cubeSize = 1.5f;

	glTranslatef(positionLeftRightChange, positionUpDownChange, 2.0f);

	glm::vec3 cubeCenter = glm::vec3(0.0, -1.0, -2.0);

	if (enableRotation)
	{
		glRotatef(angleCube, 0.0, 0.0, 1.0);
	}

	drawCube(cubeCenter.x, cubeCenter.y, cubeCenter.z, cubeSize);

	shader.~Shader();
}

void render()
{
	angleCube -= 0.15f;

	glTranslatef(positionLeftRightChange, positionUpDownChange, 2.0f);

	glm::vec3 cubeCenter = glm::vec3(-4.0, 0.0, -1.0);
	float cubeSize = 1.0;

	if (enableRotation)
	{
		glRotatef(angleCube, 0.0, -1.0, -1.0);
	}
	glLightfv(GL_LIGHT0, GL_POSITION, g_light0_pos);

	drawCube(cubeCenter.x, cubeCenter.y, cubeCenter.z, cubeSize);
}

void mirror(GLfloat  centerX, GLfloat centerY, GLfloat centerZ, GLfloat size)
{
	glVertex3f(centerX + size / 2, centerY + size / 2, centerZ); // prawy gorny
	glVertex3f(centerX - size / 2, centerY + size / 2, centerZ); // lewy gorny
	glVertex3f(centerX - size / 2, centerY - size / 2, centerZ); // lewy dolny
	glVertex3f(centerX + size / 2, centerY - size / 2, centerZ); // prawy dolny
}

void drawMirrors()
{
	static GLfloat angle = 0.0;
	GLint buffers = GL_NONE;

	glGetIntegerv(GL_DRAW_BUFFER, &buffers);

	float mirrorSize = 6.0f;

	glPushMatrix();
	glClearStencil(0x0);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 0x1, 0x1);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glDrawBuffer(GL_NONE);
	glEnable(GL_STENCIL_TEST);

	glBegin(GL_QUADS);
	mirror(mirrorPosition.x, mirrorPosition.y, mirrorPosition.z, mirrorSize);
	glEnd();

	glDrawBuffer((GLenum)buffers);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glScalef(1.0f, 1.0f, -1.0f);

	if (cos(angle * M_PI / 180.0) < 0.0)
		eqn1[2] = -1.0;
	else
		eqn1[2] = 1.0;

	glClipPlane(GL_CLIP_PLANE0, eqn1);
	glEnable(GL_CLIP_PLANE0);

	glStencilFunc(GL_EQUAL, 0x1, 0x1);

	drawMirrorReflection(shaderNumber - 1);

	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();

	glDisable(GL_STENCIL_TEST);
	glDrawBuffer(GL_NONE);

	glDrawBuffer((GLenum)buffers);

	glPushMatrix();
	render();
	glPopMatrix();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_LIGHTING);

	glColor4f(1.0f, 1.0f, 1.0f, .2f);

	glBegin(GL_QUADS);
	mirror(mirrorPosition.x, mirrorPosition.y, mirrorPosition.z, mirrorSize);
	glEnd();

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glEnable(GL_LIGHTING);

	glFlush();
	glutSwapBuffers();
}

void idleFunc()
{
	glutPostRedisplay();
}

void reshapeFunc(GLsizei w, GLsizei h)
{
	g_width = w; g_height = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 300.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 5.0f, 12.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

void displayFunc()
{
	drawMirrors();

	glFlush();
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(g_width, g_height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Mirror Sample");

	glutIdleFunc(g_rotate ? idleFunc : NULL);
	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);

	glutKeyboardFunc(keyboardFunc);

	initialize();

	glutMainLoop();

	return 0;
}