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
#include <stdlib.h>

using namespace std;

int ITER=0; //number of iterations to go thru 
Matrix4 currentMatrix; // global current matrix
list<Matrix4> stack; // matrix "stack"

float r = 1/sqrt(2);
float R = sqrt(2);
float theta = M_PI/4;
float currentScale = 1;

/* This is where rules are applied to building the string
 * The rules here are the standard assignment rules
 */
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

/* This is where rules are applied to building the string
 * The rules here are the new rules for creating different plants
 */
string applyRuleNew(string str)
{
	for(int i = 0; i < str.length(); i++)
	{
		// Rule 1
		if(str[i] == 'L')
		{
			int x = rand() % 10 + 1;
			if(i <= 5)
			{
				str.erase(i,1);
				str.insert(i, "rF[-L][+L]");
				i += 10;
			}
			else
			{
				int x = rand() % 10 + 1;
				str.erase(i,1);
				str.insert(i, "rF[-L][rF[+L]]+rF[-L]");
				i += 21;
			}
			
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

/* This is where rules are applied to building the string
 * The rules here are the new rules for creating different plants
 * This is the second set of new rules
 */
string applyRuleNewTwo(string str)
{
	for(int i = 0; i < str.length(); i++)
	{
		// Rule 1
		if(str[i] == 'A')
		{				
			str.erase(i,1);
			str.insert(i, "F[-A]B");
			i += 6;
			
		}
		// Rule 2
		else if(str[i] == 'B')
		{
			str.erase(i,1);
			str.insert(i, "F[+B]A");
			i += 6;
		}
		
	}
	
	return str;
}

/* This is where the string gets initially generated */
string generateString(string str, int x)
{
	for(int i=0; i < ITER; i++)
	{
		if(x == 1)
			str = applyRule(str);
		else if(x == 2)
			str = applyRuleNew(str);
		else if(x == 3)
			str = applyRuleNewTwo(str);
	}
	return str;
}

/* This is where letters get handled from he string 
 * No randmoization occures in this handle function
 */
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
	else if(letter == 'L' || letter == 'A' || letter == 'B')
	{
		drawLeaf();
	}
}

/* This is where letters get handled from he string 
 * Randmoization occures in this handle function
 */
void handleLetterWithRandomize(char letter)
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
		theta = M_PI/(rand() % 7 + 3);
		Matrix4 newMatrix;
		newMatrix.turnLeft(theta);
		currentMatrix = currentMatrix * newMatrix;
	}
	
	// Handle a '+' character
	else if(letter == '+')
	{
		theta = M_PI/(rand() % 7 + 3);
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

	string draw;

	/* DRAW THE FIRST TREE 
	// Generate the string pattern first
	currentMatrix.identity();
	draw = generateString("L", 1);
	
	r = 1/sqrt(2);
	R = sqrt(2);
	theta = M_PI/4;
	currentScale = 1;
	
	// Main drawing loop for tree
	for(int i = 0; i < draw.length(); i++)
	{
		char letter = draw[i];
		handleLetter(letter);
	}
	/* END OF FIRST TREE */
	
	
	/* DRAW THE SECOND TREE THIS TREE USES RANDOMIZATION */
	currentMatrix.identity();
	currentMatrix.setTranslation(200,0,0,1);
	currentScale = 1;
	srand ( time(NULL) );
	
	draw = generateString("L", 1);
	r = 1/sqrt(5);
	R = sqrt(2);
	
	// Main drawing loop for tree
	for(int i = 0; i < draw.length(); i++)
	{
		char letter = draw[i];
		handleLetterWithRandomize(letter);
	}
	/* END OF SECOND TREE */
	
	
	/* DRAW THE THIRD TREE THIS TREE USES 2ND SET OF DIFFERENT RULES */
	 currentMatrix.identity();
	 currentMatrix.setTranslation(-200,0,0,1);
	 currentScale = 1;
	 srand ( time(NULL) );
	 
	 draw = generateString("A", 3);
	 r = 1/sqrt(2);
	 R = sqrt(2);
	 theta = M_PI/7;
	 currentScale = 1;
	 
	 // Main drawing loop for tree
	 for(int i = 0; i < draw.length(); i++)
	 {
		 char letter = draw[i];
		 handleLetter(letter);
	 }
	 /* END OF THIRD TREE */
	 
	/* DRAW THE FOURTH TREE THIS TREE USES RANDOMIZATION AND 1ST SET OF DIFFERENT RULES */
	currentMatrix.identity();
	//currentMatrix.setTranslation(-100,0,0,1);
	currentScale = 1;
	srand ( time(NULL) );
	
	draw = generateString("L", 2);
	r = 1/sqrt(5);
	R = sqrt(2);
	currentScale = 1;
	
	// Main drawing loop for tree
	for(int i = 0; i < draw.length(); i++)
	{
		char letter = draw[i];
		handleLetterWithRandomize(letter);
	}
	/* END OF FOURTH TREE */

}
