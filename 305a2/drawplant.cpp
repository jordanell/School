#include <assert.h>
#include <math.h>
#include <GL/glut.h>
#include "drawplant.h"
#include "Matrix4.h"
#include "Vector4.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <list>
#include <math.h>

using namespace std;

int ITER=0; //number of iterations to go thru 
Matrix4 currentMatrix; // global current matrix
list<Matrix4> stack; // matrix "stack"

const float r = 1/sqrt(2);
const float R = sqrt(2);
const float theta = M_PI/4;
float currentScale = 1;

/* This is where rules are applied to building the string */
string applyRule(string str)
{
	for(int i = 0; i < str.length(); i++)
	{
		// Rule 1
		if(str[i] == 'L')
		{
			str.erase(i,1);
			str.insert(i, "rF[-L][+L]");
			i += 10;
		
		}
		// Rule 2
		else if(str[i] == 'F')
		{
			str.erase(i,1);
			str.insert(i, "RF");
			i += 2;
		}
	
	}
	
	return str;
}

/* This is where the string gets initially generated */
string generateString(string str)
{
	for(int i=0; i < ITER; i++)
	{
		str = applyRule(str);
	}
	return str;
}

/* This is where letters get handled fromt he string */
void handleLetter(char letter)
{
	// Handle a 'r' character
	if(letter == 'r')
	{
		Matrix4 newMatrix;
		newMatrix.setScaleY(r);
		newMatrix.setScaleX(r);
		currentMatrix = currentMatrix * newMatrix;
		currentScale *= r;
	}
	
	// Handle a 'R' character
	else if(letter == 'R')
	{
		Matrix4 newMatrix;
		newMatrix.setScaleY(R);
		newMatrix.setScaleX(R);
		currentMatrix = currentMatrix * newMatrix;
		currentScale *= R;
	}
	
	// Handle a 'F' character
	else if(letter == 'F')
	{
			drawBranch();
			// Adjust for current matrix
			Matrix4 newMatrix;
			newMatrix.setTranslation(0, 8, 0, 1); 
			currentMatrix = currentMatrix * newMatrix;
			// Reset scale after branch
			newMatrix.identity();
			newMatrix.setScaleX(1/currentScale);
			newMatrix.setScaleY(1/currentScale);
			currentMatrix = currentMatrix * newMatrix;
			currentScale = 1;
	}
	
	// Handle a '[' character
	else if(letter == '[')
	{
		stack.push_front(currentMatrix);
		glPushMatrix();
	}
	
	// Handle a ']' character
	else if(letter == ']')
	{
		currentMatrix =  stack.front();
		stack.pop_front();
		glPopMatrix();
	}
	
	// Handle a '-' character
	else if(letter == '-')
	{
		Matrix4 newMatrix;
		newMatrix.turnLeft(theta);
		currentMatrix = currentMatrix * newMatrix;
	}
	
	// Handle a '+' character
	else if(letter == '+')
	{
		Matrix4 newMatrix;
		newMatrix.turnRight(theta);
		currentMatrix = currentMatrix * newMatrix;
	}
	
	// Handle a 'L' character
	else if(letter == 'L')
	{
		drawLeaf();
	}
}

/* Takes a 3D matrix in row-major order, and loas the 4D matrix which
   does the same transformation into the OpenGL MODELVIEW matrix, in
   column-major order. */
void load3DMatrix(Matrix4 matrix)
{
	GLfloat M4D[16];
	
	M4D[0] = matrix.data[0]; M4D[1] = matrix.data[4]; M4D[2] = matrix.data[8]; M4D[3] = matrix.data[12];
	M4D[4] = matrix.data[1]; M4D[5] = matrix.data[5]; M4D[6] = matrix.data[9]; M4D[7] = matrix.data[13];
	M4D[8] = matrix.data[2]; M4D[9] = matrix.data[6]; M4D[10] = matrix.data[10]; M4D[11] = matrix.data[14];
	M4D[12] = matrix.data[3]; M4D[13] = matrix.data[7]; M4D[14] = matrix.data[11]; M4D[15] = matrix.data[15];
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(M4D);
}

void drawLeaf(void) {
  load3DMatrix(currentMatrix);

  glColor3f(0.1,0.9,0.1); 
  glBegin(GL_POLYGON);
  glVertex2f(0.0,0.0);
  glVertex2f(1.0,0.7);
  glVertex2f(1.3,1.8);
  glVertex2f(1.0,2.8);
  glVertex2f(0.0,4.0);
  glVertex2f(-1.0,2.8);
  glVertex2f(-1.3,1.8);
  glVertex2f(-1.0,0.7);
  glEnd();
}

void drawBranch(void)
{
	load3DMatrix(currentMatrix);
	
	glColor3f(0.7, 0.3, 0.2);
	glBegin(GL_POLYGON);
	glVertex2f(0.0, 0.0);
	glVertex2f(0.0, 8.0);
	glVertex2f(1.0, 8.0);
	glVertex2f(1.0, 0.0);
	glEnd();

}

void drawPlant(void) {

	/* DRAW THE FIRST TREE */
	// Generate the string pattern first
	currentMatrix.identity();
	string draw = generateString("L");
	
	cout << "Drawing the tree: " << draw << "\n";
	
	// Main drawing loop for tree
	for(int i = 0; i < draw.length(); i++)
	{
		char letter = draw[i];
		handleLetter(letter);
	}

}
