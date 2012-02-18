/*********
 Arcball transformation Class
 Written by Jordan Ell
 Copyright 2012
 *********/

#include "Arcball.h"

// Some extra arrays
GLdouble glp[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
GLdouble glm[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
int glv[4] = {0,0,640,480};

Arcball::Arcball()
{
	width = 400;
	height = 400;
	
	init();
}

Arcball::Arcball(int w, int h)
{
	width = w;
	height = h;
	
	init();
}

Arcball::~Arcball()
{
	// Nothing to destruct
}

/*
 * Initialize some values for the Arcball
 */
void Arcball::init()
{
	// Initialize floats
	zoom = 1.0;
	zoom2 = 1.0;
	sphere = 1.0;
	sphere2 = 1.0;
	edge = 1.0;
	planar = false;
	planedist = 0.5;
	
	//Initialize Vectors
	start = Vector4(0,0,1,1);
	curr = Vector4(0,0,1,1);
	eye = Vector4(0,0,1,1);
	eyedir = Vector4(0,0,1,1);
	up = Vector4(0,1,0,1);
	aOut = Vector4(1,0,0,1);
	
	//Initialize Matricies
	transform = Matrix4();
	lastRot = Matrix4();
	thisRot = Matrix4();
	
}

/*
 * Resets the bounds of the Arcball
 */
void Arcball::setBounds(int w, int h)
{
	width = w;
	height = h;
}

/*
 * Sets up the arcball to work with the current scene
 */
void Arcball::setZoom(float radius, Vector4 eye, Vector4 up)
{
	this->eye = eye;
	zoom2 = this->eye * this->eye;
	zoom = sqrt(zoom2);
	sphere = radius;
	sphere2 = sphere * sphere;
	eyedir = this->eye * (1.0/zoom);
	edge = sphere2 / zoom;
	
	if(sphere <= 0.0)
	{
		planar = true;
		up = up;
		aOut = eyedir^up;
		planedist = (0.0 - sphere) * zoom;
	}
	else
		planar = false;
		
	glGetDoublev(GL_PROJECTION_MATRIX,glp);
	glGetIntegerv(GL_VIEWPORT,glv);
}

/*
 * Loads the transform matrix into OpenGL
 * Has to convert to column order matrix
 */
void Arcball::applyTransform()
{
	glMatrixMode(GL_MODELVIEW);
	glMultMatrixf(transform.data);
}

Vector4 Arcball::sphereCoords(GLdouble mx, GLdouble my)
{
	GLdouble ax,ay,az;
	gluUnProject(mx,my,0,glm,glp,glv,&ax,&ay,&az);
	Vector4 m = Vector4(ax, ay, az, 1) - eye;
	
	GLfloat a = m*m;
	GLfloat b = eye*m;
	GLfloat root = b*b - a*(zoom2 - sphere2);
	
	if(root <= 0)
		return edgeCoords(m);
		
	GLfloat t = (0.0 - b - sqrt(root)) / a;
	Vector4 value = eye;
	value += (m*t);
	value.normalize();
	return value;
}

Vector4 Arcball::planarCoords(GLdouble mx, GLdouble my)
{
	GLdouble ax,ay,az;
	gluUnProject(mx,my,0,glm,glp,glv,&ax,&ay,&az);
	Vector4 m = Vector4(ax, ay, az, 1) - eye;
	
	GLfloat t = (planedist - zoom) / (eyedir * m);
	Vector4 d = eye;
	d += (m*t);
	
	return Vector4(d*up, d*aOut, 0.0, 1);
}

Vector4 Arcball::edgeCoords(Vector4 m)
{
	float t = (edge - zoom) / (eyedir * m);
	Vector4 a = eye;
	a += (m*t);
	Vector4 c = eyedir * edge;
	c -= a;
	
	float ac = a*c;
	float c2 = c*c;
	float q = (0.0-ac-sqrt(ac*ac - c2*((a*a)-sphere2))) / c2;
	
	Vector4 value = a;
	value += c*q;
	value.normalize();
	return value;
}
	
void Arcball::click(GLdouble mx, GLdouble my)
{
	lastRot = transform;
	if(planar)
		start = planarCoords(mx, my);
	else
		start = sphereCoords(mx, my);
}

void Arcball::drag(GLdouble mx, GLdouble my)
{
	if(planar)
	{
		curr = planarCoords(mx, my);
		if(curr == start)
			return;
		
		Vector4 d = curr - start;
		
		GLfloat angle = d.norm() * 0.5;
		GLfloat cosa = cos(angle);
		GLfloat sina = sin(angle);
		
		Vector4 p = (aOut*d.data[0]);
		p -= (up*d.data[1]);
		p.normalize();
		p *= sina;
		
		quaternion(p.x(), p.y(), p.z(), cosa);
		transform = lastRot * thisRot;
		lastRot = transform;
		start = curr;
	}
	else
	{
		curr = sphereCoords(mx, my);
		if(curr == start)
		{
			transform = lastRot;
			return;
		}
		
		GLfloat cos2a = start*curr;
		GLfloat sina = sqrt((1.0-cos2a)*0.5);
		GLfloat cosa = sqrt((1.0+cos2a)*0.5);
		Vector4 cross = start^curr;
		cross.normalize();
		cross *= sina;
		
		quaternion(cross.x(), cross.y(), cross.z(), cosa);
		transform = lastRot * thisRot;
	}
}

/*
 * This create the transformation quaternion
 * This also converts the tranformation matrix into
 * column order which OpenGL needs
 */
void Arcball::quaternion(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	GLfloat x2 = x*x;
	GLfloat y2 = y*y;
	GLfloat z2 = z*z;
	GLfloat xy = x*y;
	GLfloat xz = x*z;
	GLfloat yz = y*z;
	GLfloat wx = w*x;
	GLfloat wy = w*y;
	GLfloat wz = w*z;
	
	thisRot.data[0] = 1 - 2*y2 - 2*z2;
	thisRot.data[1] = 2*xy + 2*wz;
	thisRot.data[2] = 2*xz - 2*wy;
	
	thisRot.data[4] = 2*xy - 2*wz;
	thisRot.data[5] = 1 - 2*x2 - 2*z2;
	thisRot.data[6] = 2*yz + 2*wx;
	
	thisRot.data[8] = 2*xz + 2*wy;
	thisRot.data[9] = 2*yz - 2*wx;
	thisRot.data[10]= 1 - 2*x2 - 2*y2;
}

