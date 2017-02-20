// Based on project by Kamil Szostek: http://home.agh.edu.pl/~szostek

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <time.h>
#include "GL\glew.h"
#include "GL\freeglut.h"
#include "glm/vec3.hpp" 
#include "glm/vec4.hpp" 
#include "glm/mat4x4.hpp" 
#include "glm/gtc/matrix_transform.hpp" 
#include "shaderLoader.h" 
#include "GameLife.h"

struct Point3
{
	float x;
	float y;
	float z;
};

int screenWidth = 640;
int screenHeight = 480;

int mousePositionX;
int mousePositionY;
int mouseButtonClicked;

double cameraX = 90;
double cameraZ = 90;
double cameraD = -10.0;
double prevCameraX;
double prevCameraZ;
double prevCameraD;

double cameraAngle = 20;

glm::mat4 M;
glm::mat4 V;
glm::mat4 P;

GLuint vboId[2] = { 0 };
GLuint programID = 0;

int numberOfPoints = 1;

GameLife* gra;
int pause = 0;

int accumulator = 0;
clock_t elapsed = 0;
clock_t prev2;
clock_t t;

float stepCount = 0;

void mouseClickHandler(int button, int state, int x, int y)
{
	mouseButtonClicked = button;
	switch (state)
	{
	case GLUT_UP:
		break;
	case GLUT_DOWN:
		mousePositionX = x;
		mousePositionY = y;
		prevCameraX = cameraX;
		prevCameraZ = cameraZ;
		prevCameraD = cameraD;
		break;

	}
}

void mouseMoveHandler(int x, int y)
{
	if (mouseButtonClicked == GLUT_LEFT_BUTTON)
	{
		cameraX = prevCameraX - (mousePositionX - x) * 0.1;
		cameraZ = prevCameraZ - (mousePositionY - y) * 0.1;
	}
	if (mouseButtonClicked == GLUT_RIGHT_BUTTON)
	{
		cameraD = prevCameraD + (mousePositionY - y) * 0.1;
	}

}

void keyboardHandler(GLubyte key, int x, int y)
{
	switch (key) {
	case ' ':
		pause = !pause;
		break;
	case 27:
		exit(1);
		break;
	}
}

void resizeHandler(int width, int height)
{
	screenWidth = width;
	screenHeight = height;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screenWidth, screenHeight);

	P = glm::perspective(glm::radians(60.0f), (GLfloat)screenWidth / (GLfloat)screenHeight, 0.01f, 1000.0f);

	glutPostRedisplay();
}

void idle()
{
	char buf[60];
	prev2 = t;
	t = clock();

	accumulator++;
	clock_t diff = t - prev2;
	elapsed += diff;

	if (elapsed >= 1000)
	{
		sprintf_s(buf, 60, "VBO ON: %.1lf FPS (%.1lf ms)", (double)accumulator / elapsed * 1000.0, (float)diff);
		glutSetWindowTitle(buf);

		accumulator = 0;
		elapsed = 0;
	}
	glutPostRedisplay();
}

void drawVBO(float r, float g, float b, float a, float height)
{
	glUseProgram(programID); //u¿yj programu, czyli naszego shadera	

	glm::mat4 MVP = P*V*M;
	GLuint MVP_id = glGetUniformLocation(programID, "MVP"); // pobierz lokalizacjê zmiennej 'uniform' "MV" w programie
	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &(MVP[0][0]));	   // wyœlij tablicê mv do lokalizacji "MV", która jest typu mat4	

	float tab[4] = { r,g,b,a };
	GLuint color_id = glGetUniformLocation(programID, "color");
	glUniform4fv(color_id, 1, tab);

	GLuint height_id = glGetUniformLocation(programID, "height");
	glUniform1f(height_id, height);


	// GLuint color_id = glGetUniformLocation(programID, "color");
	// glUniform3fv(color_id, 1, tab);

	glEnableVertexAttribArray(0); // pierwszy buform atrybuów: wierzcho³ki
	glBindBuffer(GL_ARRAY_BUFFER, vboId[0]);
	glVertexAttribPointer(
		0,                  // atrybut 0. musi odpowiadaæ atrybutowi w programie shader
		3,                  // wielkoœæ (x,y,z)
		GL_FLOAT,           // typ
		GL_FALSE,           // czy znormalizowany [0-1]?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// rysowanie
	glDrawArrays(GL_POINTS, 0, numberOfPoints); // Zaczynamy od 0 i rysujemy wszystkie wierzcho³ki

	glDisableVertexAttribArray(0);  // wy³¹czamy u¿ywanie zdefiniowanych atrybutów	
	glUseProgram(0);				// wy³¹czamy u¿ywanie programu (teraz mo¿emy znów rysowaæ po staremu)
}

void draw(void)
{
	int i = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Kasowanie ekranu

	V = glm::mat4(1.0f); //glLoadIdentity();
	V = glm::translate(V, glm::vec3(0, 0, cameraD)); //glTranslated(0.0f, 0.0f, kameraD);
	V = glm::rotate(V, (float)glm::radians(cameraZ), glm::vec3(1, 0, 0)); //glRotatef(kameraZ, 1.0, 0.0, 0.0);
	V = glm::rotate(V, (float)glm::radians(cameraX), glm::vec3(0, 1, 0)); //glRotatef(kameraX, 0.0, 1.0, 0.0);

	for (int i = 0; i <gra->get_width(); i++) {
		for (int j = 0; j < gra->get_height(); j++) {
			int isAlive = gra->is_alive(i, j);
			int wasAlive = gra->was_alive(i, j);
			if (isAlive || wasAlive) {
				M = glm::mat4(1.0f);
				M = glm::translate(M, glm::vec3(-gra->get_width() / 10.0f + i*0.2, 0, -gra->get_height() / 10 + j*0.2));

				int noOfNeighbours = gra->count_neighbours(i, j);
				float r =  1.0f - (noOfNeighbours <= 4 ? noOfNeighbours / 4.0 : 1.0f);
				float b = (noOfNeighbours >= 4 ? (noOfNeighbours - 3) / 4.0 : 0.0f);
				float g = (r + b < 1.0f ? 1 - r - b : 0.0f);

				float height;
				float a;

				if (isAlive && wasAlive) {
					height = 0.5f;
					a = 1.0f;
				}
				else if (isAlive && !wasAlive) {
					height = stepCount / 200.0f;
					a = stepCount / 100.0f;
				}
				else if (!isAlive && wasAlive) {
					height = (0.5f - stepCount / 200.0f);
					a = (1.0f - stepCount / 100.0f);
				}
				else {
					height = 0.0f;
					a = 0.38f;
				}
				drawVBO(r, g, b, a, height);
			}
		}
	}
	
	glFlush();
	glutSwapBuffers();

	if (!pause) {
		stepCount+=0.25;
		if (stepCount > 100) {
			stepCount = 0;
			gra->next_step();
		}
	}
}

Point3* point() {
	numberOfPoints = 1;
	Point3* verticles = (Point3*)malloc( sizeof(Point3));

	verticles[0].x = 0;
	verticles[0].y = 0;
	verticles[0].z = 0;

	return verticles;
}

void createVBO()
{
	Point3 *vertices = point();

	if (vboId[0] == 0)
		glGenBuffers(1, &vboId[0]); // tworzenie bufora na karcie. Na hoœcie dostajemy tylko ID tego miejsca.

	glBindBuffer(GL_ARRAY_BUFFER, vboId[0]); // ustawianie bufora na aktualnie modyfikowany
	glBufferData(GL_ARRAY_BUFFER, numberOfPoints * sizeof(Point3), vertices, GL_STATIC_DRAW); //wys³anie wierzcho³ków do VRAM, czyli do karty graficznej. 

	free(vertices); // usuwamy, bo wys³aliœmy ju¿ na kartê graficzn¹	
}

int main(int argc, char **argv)
{
	gra = new GameLife(40, 40);

	FILE* plik1;

	if ((plik1 = fopen("stan_poczatkowy.txt", "r")) != NULL) {
	//	gra->read_state(plik1);
		fclose(plik1);
	}
	
	GLuint VertexArrayID;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("VBO TEST");

	glewInit(); //init rozszerzeszeñ OpenGL z biblioteki GLEW

	glutDisplayFunc(draw);
	glutIdleFunc(idle);
	glutReshapeFunc(resizeHandler);

	glutKeyboardFunc(keyboardHandler);
	glutMouseFunc(mouseClickHandler);
	glutMotionFunc(mouseMoveHandler);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND); // transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPointSize(10.0f);

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	programID = loadShaders("vertex_shader.glsl", "fragment_shader.glsl");
	for (int i = 0; i < 40; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			gra->cells_state[i][j] = rand() % 2;
		}
	}
	createVBO();

	glutMainLoop();
	return(0);
}
