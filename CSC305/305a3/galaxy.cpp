/*
 * This file is the main control flow of the arcball system.
 * It handles input and produces the output.
 * Date: 14/02/2012
 * Author: Jordan Ell
 */

#include <GL/glut.h>
#include <cstdlib>
#include <math.h>
#include "Vector4.h"
#include "Arcball.h"
#include <iostream>

// Sphere settings
const int NUM_FACE_COLORS = 4;
const float SPHERE_RADIUS = 5.0f;
const int SPHERE_LAT_SLICES = 12;
const int SPHERE_LONG_SLICES = 24;

// Star settings
const int NUM_STARS = 256;
Vector4 stars[NUM_STARS];

// Window settings
int width, height;
float aspectRatio = 1.0f;

// 3D viewing settings
Vector4 eye(0.0f, 0.0f, -20.0f, 1.0f);
Vector4 center(0.0f, 0.0f, 0.0f, 1.0f);
Vector4 up(0.0f, 1.0f, 0.0f, 1.0f);
Arcball arcball;

// Wireframe settings
bool wire = false;

/* 
 * Resets the view to it's initial state
 */
void reset(int w, int h)
{
	width = w;
	height = h;
	aspectRatio = (float)width/(float)height;
	
	arcball = Arcball(400,400);
	glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 50.0f, aspectRatio, 1.0f, 50.0f );
    gluLookAt(
			  eye.x(), eye.y(), eye.z(),
			  center.x(), center.y(), center.z(),
			  up.x(), up.y(), up.z() );
	arcball.setBounds(w,h);
	arcball.setZoom(SPHERE_RADIUS, eye, up);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

float randFloat()
{
	return ((1.0f / 127.f) * (((float)(rand() & 255)) - 128.0f)) ;
}

/*
 * Generate the random positioning of the stars
 */
void generateStars()
{
	for(int i=0; i<NUM_STARS; i++)
	{
		Vector4 vec(randFloat(),randFloat(),randFloat(), 1.0f);
		vec.normalize();
		vec = vec * 2.0f;
		stars[i] = vec;
	}
}

/*
 * This function initializes the scene
 */
void init()
{
	generateStars();
	arcball = Arcball(400, 400);
}

/*
 * This function returns a rotation vector from vector
 * v around the angle ang around the x axis
 */
Vector4 rotateX(Vector4 v, float sinAng, float cosAng)
{
	return Vector4(
		v.x(),
		(v.y() * cosAng) + (v.z() * sinAng),
		(v.z() * cosAng) - (v.y() * sinAng),
		1.0f
	);
}

/*
 * This function returns a rotation vector from vector
 * v around the angle ang around the Y axis
 */
Vector4 rotateY(Vector4 v, float sinAng, float cosAng)
{
	return Vector4(
		(v.x() * cosAng) + (v.z() * sinAng),
		v.y(),
		(v.z() * cosAng) - (v.x() * sinAng),
		1.0f
		);
}

/*
 * Draw the main sphere here
 */
void drawSphere()
{
	// Set up the colors
	Vector4 FACE_COLORS[NUM_FACE_COLORS] = {
		Vector4(1.0f, 0.0f, 0.0f, 1.0f),
		Vector4(0.0f, 1.0f, 0.0f, 1.0f),
		Vector4(0.0f, 0.0f, 1.0f, 1.0f),
		Vector4(1.0f, 1.0f, 0.0f, 1.0f)
	};
	
	// Set up sphere angles
	float latAngle = M_PI/(float)SPHERE_LAT_SLICES;
	float longAngle = 2.0f * M_PI/(float)SPHERE_LONG_SLICES;
	
	float sinLat = sin(latAngle);
	float cosLat = cos(latAngle);
	float sinLong = sin(longAngle);
	float cosLong = cos(longAngle);
	
	//Draw the sphere
	glBegin(GL_QUADS);
	
	Vector4 lat0(0.0f, SPHERE_RADIUS, 0.0f, 1.0f);
	
	for(int y=0; y < SPHERE_LAT_SLICES; y++)
	{
		Vector4 lat1 = rotateX(lat0, sinLat, cosLat);
		Vector4 long00 = lat0;
		Vector4 long10 = lat1;
		
		for(int x=0; x < SPHERE_LONG_SLICES; x++)
		{
			Vector4 long01 = rotateY(long00, sinLong, cosLong);
			Vector4 long11 = rotateY(long10, sinLong, cosLong);
			
			Vector4 color = FACE_COLORS[(x+y)%NUM_FACE_COLORS];
			glColor4f(color.x(), color.y(), color.z(), 1.0f);
			
			glVertex3f(long00.x(), long00.y(), long00.z());
			glVertex3f(long10.x(), long10.y(), long10.z());
			glVertex3f(long11.x(), long11.y(), long11.z());
			glVertex3f(long01.x(), long01.y(), long01.z());
			
			long00 = long01;
			long10 = long11;
		}
		lat0 = lat1;
	}
	glEnd();
}

/*
 * This function draws the stars
 */
void drawStars()
{
	glBegin(GL_POINTS);
	glColor3f(1.0f, 1.0f, 1.0f);
	for(int i=0; i < NUM_STARS; i++)
		glVertex3f(stars[i].x(), stars[i].y(), stars[i].z());
	glEnd();
}

/*
 * Draw the wire frame model of the sphere
 */
void drawSphereWire()
{
	glColor3f(1.0f,0.0f,0.0f);
	glutWireSphere(SPHERE_RADIUS,SPHERE_LONG_SLICES,SPHERE_LAT_SLICES);
}

/*
 * This function handles the drawing of the entire scene
 */
void drawScene()
{
	// Draw the stars
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glTranslatef(eye.x(), eye.y(), eye.z());
	arcball.applyTransform();
	drawStars();
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	
	// Draw the planet
	glPushMatrix();
	arcball.applyTransform();
	if(wire)
		drawSphereWire();
	else
		drawSphere();
	glPopMatrix();
}

/*
 * This is the main display function
 */
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	
	drawScene();
	
    glutSwapBuffers();
}

void resize(int w, int h)
{
	reset(w, h);
}

/*
 * When the program is idling
 */
void idle()
{
	glutPostRedisplay();
}

/*
 * When the user presses the mouse button
 */
void mouseButton(int button, int state, int x, int y)
{
	int invertY = (height - y) - 1;
	if(state == GLUT_DOWN)
		arcball.click(x, invertY);
}

/*
 * When the user moves the mouse
 */
void mouseMotion(int x, int y)
{
	int invertY = (height - y) - 1;
	arcball.drag(x, invertY);
}

/*
 * Handle keyboard input here
 */
void key(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'q':
			exit(0);
			break;
		case 'r':
			reset(width, height);
			break;
		case 'w':
			wire = !wire;
			break;
		default:
			break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(400,400);
	glutInitWindowPosition(100,100);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Arball Galaxy");
	
	// Call back functions
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(key);
	glutIdleFunc(idle);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	init();
	
	glutMainLoop();
	
	return 0;
}
