#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
# define PI 3.14159265358979323846
using namespace std;

void update(int);
void initVars(void);
bool start_flag = 0;
long long start_time;
double angle = 45;
double initial_velocity = 50;
double ball_x = -11;
double ball_y = -5.5;
double vx, vy, range, elapsed_time_s, x = ball_x, y = ball_y;
double gravity = 9.8;
long long elapsed_time, total_time;
double pixel_scaling = 20;
vector<pair<double, double>> points;

//Called when a key is pressed
void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
	case 27: //Escape key to exit
		exit(0);

	//space key to start
	case 32: {
		if (!start_flag) {
			start_flag = 1;
			start_time = glutGet(GLUT_ELAPSED_TIME);
			glutTimerFunc(16.68, update, 0); //Add a timer
			break;
		}
	}

	//a key to increase angle
	case 97: {
		if (!start_flag) {
			++angle;
			if (angle > 90)
				angle = 90;
			initVars();
		}
		break;
	}

	//s key to decrease angle
	case 115: {
		if (!start_flag) {
			--angle;
			if (angle < 10)
				angle = 10;
			initVars();
		}
		break;
	}
	
	//d key to increase initial velocity
	case 100: {
		if (!start_flag) {
			++initial_velocity;
			if (initial_velocity > 65)
				initial_velocity = 65;
			initVars();
		}
		break;
	}

	//f key to decrease initial velocity
	case 102: {
		if (!start_flag) {
			--initial_velocity;
			if (initial_velocity < 40)
				initial_velocity = 40;
			initVars();
		}
		break;
	}
	}
}

//Initializes 3D rendering
void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.8941176470588236, 0.8941176470588236, 0.8941176470588236, 1);	//Change the background
	gluOrtho2D(0, 1000, 0, 600);
	glMatrixMode(GL_PROJECTION);
}

//Called when the window is resized
void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

//for drawing the ball
void drawCircle(double r) {
	double theta;
	glColor3f(0, 0.5254901960784314, 0.49019607843137253);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i++) {
		theta = i * PI / 180;
		glVertex2d(r * cos(theta) + ball_x, r * sin(theta) + ball_y);
	}
	glEnd();
}

//for drawing angle lines
void drawAngle() {
	glPushAttrib(GL_ENABLE_BIT);
	glLineStipple(1, 0xAAAA);
	glLineWidth(3);
	glColor3f(1, 0, 0);
	glEnable(GL_LINE_STIPPLE);

	glBegin(GL_LINES);
	glVertex2f(ball_x, ball_y - 0.5);
	glVertex2f(ball_x + 2, ball_y - 0.5);
	glEnd();

	glPushMatrix();
	glTranslated(ball_x, ball_y - 0.5, 0);
	glRotated(angle, 0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex2f(0, 0);
	glVertex2f(2, 0);
	glEnd();
	glTranslated(-ball_x, -ball_y + 0.5, 0);
	glPopMatrix();

	glPopAttrib();
}

//for initializing motion variables
void initVars() {
	vx = initial_velocity * cos(angle * PI / 180);
	vy = initial_velocity * sin(angle * PI / 180);
	//in milliseconds
	total_time = 2 * vy / gravity * 1000;
	range = vx * total_time / 1000;
	points.clear();
}

void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	glLoadIdentity(); //Reset the drawing perspective

	glTranslated(0.0f, 0.0f, -20.0f); //Move forward 20 units

	glPushMatrix();
	if (y >= ball_y)
		glTranslated(x / pixel_scaling, y / pixel_scaling, 0);
	else
		glTranslated(range / pixel_scaling, ball_y / pixel_scaling, 0);
	drawCircle(0.5);
	glPopMatrix();

	drawAngle();

	glColor3i(0, 0, 0);
	glPointSize(5);
	glPushMatrix();
	glTranslated(ball_x, ball_y, 0);
	glBegin(GL_POINTS);
	int l = points.size();
	for (int i = 0; i < l; i++) {
		glVertex2d(points[i].first / pixel_scaling, points[i].second / pixel_scaling);
	}
	glEnd();
	glPopMatrix();

	//glTranslated(0.0f, 0.0f, 20.0f);

	glutSwapBuffers();
}

void update(int value) {
	//get elapsed time since the start of the program
	elapsed_time = glutGet(GLUT_ELAPSED_TIME) - start_time;
	elapsed_time_s = elapsed_time / 1000;

	if (elapsed_time <= total_time) {
		x = vx * elapsed_time_s;
		y = vy * elapsed_time_s - 0.5 * gravity * elapsed_time_s * elapsed_time_s;
	}
	points.push_back({ x, y });
	glutPostRedisplay(); //Tell GLUT that the display has changed
	//Tell GLUT to call update again in 25 milliseconds
	glutTimerFunc(16.68, update, 0);
}

int main(int argc, char** argv) {
	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1000, 600);

	//Create the window
	glutCreateWindow("Projectile motion");
	initRendering();

	//Set handler functions
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	initVars();
	glutMainLoop();
	return 0;
}
