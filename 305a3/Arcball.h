/*********
 Arcball transformation Class
 Written by Jordan Ell
 Copyright 2012
 *********/


#ifndef ARCBALL_H
#define ARCBALL_H

#include "math.h"
#include "Matrix4.h"
#include "Vector4.h"
#include <GL/glut.h>
#include <iostream>

class Arcball {
	public:
		Matrix4 transform;
		Matrix4 lastRot;
		Matrix4 thisRot;
	
		int width, height;
	
		Arcball();
		Arcball(int w, int h);
		~Arcball();
	
		void setBounds(int w, int h);
		void applyTransform();
		
		void setZoom(float radius, Vector4 eye, Vector4 up);
		
		void click(GLdouble mx, GLdouble my);
		void drag(GLdouble mx, GLdouble my);
		
	private:
		GLfloat zoom;
		GLfloat zoom2;
		GLfloat sphere;
		GLfloat sphere2;
		GLfloat edge;
		bool planar;
		GLfloat planedist;
		
		Vector4 start;
		Vector4 curr;
		Vector4 eye;
		Vector4 eyedir;
		Vector4 up;
		Vector4 aOut;
	
		void init();
		Vector4 sphereCoords(GLdouble mx, GLdouble my);
		Vector4 planarCoords(GLdouble mx, GLdouble my);
		Vector4 edgeCoords(Vector4 m);
		
		void quaternion(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
};

#endif  

