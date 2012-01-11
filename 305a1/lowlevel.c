#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include "lowlevel.h"

/* Variables local to this file */

 int W; /* width of frame buffer */
 int H; /* height of frame buffer */


color current; /* current color */
vertex* v[3];  /* the array of vertices we store away */
int ind = 0;  /* index of current vertex in the array */

/*****************************************************************************/
/* Fake frame buffer - Use an array of 3 GLubytes per pixel as an imitiation
   frame buffer. We'll color the pixels of this array to draw a triangle */

/* pointer to fake frame buffer */
GLubyte* fb;


/* Allocate storage for fake frame buffer, and fill it with all white */
void initFakeFrameBuffer(int w, int h) {
  int i;

  fb = (GLubyte*) malloc(W*H*3*sizeof(GLubyte));
  for (i=0; i<W*H*3; i++) {
    fb[i]=0x00;
  }
}

/* Draws fake framebuffer into OpenGL window and releases storage */
void outputFrame(void) {
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glRasterPos3f(0.0,0.0,0.0);
  glDrawPixels(W,H,GL_RGB,GL_UNSIGNED_BYTE,fb);
  free(fb);
}

/*****************************************************************************/
/* Store color and vertex information recieved from the main program */

/* Take a new color and stores it in global variables */
void setColor3(GLubyte newr, GLubyte newg, GLubyte newb) {
  current.red = newr;
  current.green = newg;
  current.blue = newb;
}

void setColor(color new_color){
  current.red = new_color.red;
  current.green = new_color.green;
  current.blue = new_color.blue;
}

/* Make a vertex */
vertex* newVertex(int x, int y) {
  vertex* v;
  v = (vertex*) malloc(sizeof(vertex));
  v->x = x;
  v->y = y;
  return(v);
}

/* Save a triangle vertex */
void setVertex(int x, int y) {
  if (ind < 3) {
    v[ind] = newVertex(x,y);
	v[ind]->v_color.red = current.red;
	v[ind]->v_color.green = current.green;
	v[ind]->v_color.blue = current.blue;
    ind++;
  }
}

/* Start a new triangle */
void beginTriangle() {
  ind = 0;
}

/*****************************************************************************/
/* Actual triangle drawing code */

/* Makes the data structure for the edge between bottom point
   b and top point t */ 

edge* newEdge(vertex* b, vertex* t) {
  edge* e;
  int dx,dy;

  e = (edge*) malloc(sizeof(edge));
  e->x = b->x; /* edge passes through bottom vertex */
  /* D = ax + by +c - must be zero since edge goes through bottom vertex */
  e->D = 0;    

  dx = t->x - b->x;
  dy = t->y - b->y;
  if (dy == 0) {} /* doesn't matter, edge will never be used */
  else {
    e->xSmallStep = dx/dy; /* integer divide */
    if (dx < 0) {  /* positive or negative slope? */
      e->xSmallStep --;
    }
    e->a = -dy;
    e->incDSmall = e->a * e->xSmallStep + dx; 
  }
  return(e);
}

/* Called in second-to-innermost loop: 
   Moves the x position of the edge at each row */
void yInc(edge* e) {

  e->x += e->xSmallStep;
  e->D += e->incDSmall;  /* edge moves, increasing error */
  if (e->D > 0) {        /* lots of error, take a big step */
    e->x++;              /* pixel moves a bit more */
    e->D += e->a;        /* decreasing error (a is negative) */
  }
}

/* Prints a color for debugging */
void printColor(color* color)
{
	printf("Red: %i Green: %i, Blue: %i\n", color->red, color->green, color->blue);
}

void printVertex(vertex* vertex)
{
	printf("X: %i Y: %i\n", vertex->x, vertex->y);
	printColor(&vertex->v_color);
}

/* Interpolate a color based on two end verticies and a given point along their line using floats */
color* interpolateColor(vertex* left, vertex* right, int x)
{
	color* nColor;
	nColor = (color*) malloc(sizeof(color));
	
	GLubyte red = (float)left->v_color.red + ((float)(x - left->x) * (((float)right->v_color.red - (float)left->v_color.red)/((float)right->x - (float)left->x)));
	GLubyte green = (float)left->v_color.green + ((float)(x - left->x) * (((float)right->v_color.green - (float)left->v_color.green)/((float)right->x - (float)left->x)));
	GLubyte blue = (float)left->v_color.blue + ((float)(x - left->x) * (((float)right->v_color.blue - (float)left->v_color.blue)/((float)right->x - (float)left->x)));
	
	nColor->red = red;
	nColor->green = green;
	nColor->blue = blue;
	
	return nColor;
}

/* Bresenham's algorithm which does the actual integer interpolation using integers and addition */
int bresenhamAlgorithm(int p1x, int p1y, int p2x, int p2y, int dy, int dx, int dy2, int dx2, int dy2_minus_dx2, int dy2_plus_dx2, int xm)
{
	int F, x, y;
	
	if(dy >= 0)
	{
		// Case 1: 0 <= m <= 1
		if(dy <= dx)
		{
			F = dy2 - dx;
			
			x = p1x;
			y = p1y;
			while(x <= p2x)
			{
				if(x == xm)
					return y;
				if(F <= 0)
					F += dy2;
				else
				{
					y++;
					F += dy2_minus_dx2;
				}
				x++;
			}
		}
		// Case 2: 1 < m < INF
		else
		{
			F = dx2 - dy;
			
			y = p1y;
			x = p1x;
			while(y <= p2y)
			{
				if(x == xm)
					return y;
				if(F <= 0)
					F += dx2;
				else
				{
					x++;
					F -= dy2_minus_dx2;
				}
				y++;
			}
		}
	}
	else
	{
		// Case 3: -1 <= m < 0
		if(dx >= -dy)
		{
			F = -dy2 - dx;
			
			x = p1x;
			y = p1y;
			while(x <= p2x)
			{
				if(x == xm)
					return y;
				if(F <= 0)
					F -= dy2;
				else
				{
					y--;
					F -= dy2_plus_dx2;
				}
				x++;
			}
		}
		// Case 4: -INF < m < -1
		else
		{
			F = dx2 + dy;
			
			y = p1y;
			x = p1x;
			while(y >= p2y)
			{
				if(x == xm)
					return y;
				if(F <= 0)
					F += dx2;
				else
				{
					x++;
					F += dy2_plus_dx2;
				}
				y--;
			}
		}
	}
}

/* Interpolate a color based on two end verticies and a given point along their line using integers and addition */
color* interpolateColorInt(vertex* left, vertex* right, int x)
{
	color* nColor;
	nColor = (color*) malloc(sizeof(color));
	
	if(left->x > right->x)
	{
		vertex* templeft = left;
		vertex* tempright = right;
		left = tempright;
		right = templeft;
	}
	
	// Set up all the variables
	int dred = right->v_color.red - left->v_color.red;
	int dgreen = right->v_color.green - left->v_color.green;
	int dblue = right->v_color.blue - left->v_color.blue;
	
	int dx = right->x - left->x;
	
	int dred2 = (dred << 1);
	int dgreen2 = (dgreen << 1);
	int dblue2 = (dblue << 1);
	
	int dx2 = (dx << 1);
	
	int dred2_minus_dx2 = dred2 - dx2;
	int dred2_plus_dx2 = dred2 + dx2;
	int dgreen2_minus_dx2 = dgreen2 - dx2;
	int dgreen2_plus_dx2 = dgreen2 + dx2;
	int dblue2_minus_dx2 = dblue2 - dx2;
	int dblue2_plus_dx2 = dblue2 + dx2;
	
	
	// Call the actual algorithm
	GLubyte red = bresenhamAlgorithm(left->x, left->v_color.red, right->x, right->v_color.red, dred, dx, dred2, dx2, dred2_minus_dx2, dred2_plus_dx2, x);
	GLubyte green = bresenhamAlgorithm(left->x, left->v_color.green, right->x, right->v_color.green, dgreen, dx, dgreen2, dx2, dgreen2_minus_dx2, dgreen2_plus_dx2, x);
	GLubyte blue = bresenhamAlgorithm(left->x, left->v_color.blue, right->x, right->v_color.blue, dblue, dx, dblue2, dx2, dblue2_minus_dx2, dblue2_plus_dx2, x);
	
	nColor->red = red;
	nColor->green = green;
	nColor->blue = blue;
	
	return nColor;
}


/* Innermost loop - draws one row of a triangle */
void drawRow(int left, int right, int y) {
  int x = left;

  while (x < right) {
    /* draw pixel using macro (see lowlevel.h) */
	// ADD CODE HERE
	
    DRAWPIX(x,y,current.red,current.green,current.blue);  
    x++;
  }
}

/* Draws a row of the triangle based on verticies */
void drawRowByVertex(vertex* left, vertex* right, int y)
{
	int x = left->x;
	
	while (x < right->x)
	{
		color* nColor = interpolateColorInt(left, right, x);
		DRAWPIX(x,y,nColor->red,nColor->green,nColor->blue);
		free(nColor);
		x++;
	}
}

/* Draws a triangle with bottom vertex b, top vertex t, and point
   m to the left of line from b to t */
void drawLeftTriangle(vertex* b, vertex* t, vertex* m) {
  edge* lEdge;
  edge* rEdge;
  int y;

  /* draw rows from bottom-middle edge to bottom-top edge */
  lEdge = newEdge(b,m);
  rEdge = newEdge(b,t);

  for (y=b->y; y<m->y; y++) {
    //drawRow(lEdge->x,rEdge->x,y);
	
	/* Create the left vertex */
	vertex* leftV = (vertex*) malloc(sizeof(vertex));
	leftV->x = lEdge->x;
	leftV->y = y;
	color* nColor = interpolateColorInt(b, m, lEdge->x);
	leftV->v_color = *nColor;
	
	/* Create the right vertex */
	vertex* rightV = (vertex*) malloc(sizeof(vertex));
	rightV->x = rEdge->x;
	rightV->y = y;
	color* nColor2 = interpolateColorInt(b, t, rEdge->x);
	rightV->v_color = *nColor2;
	
	/* Draw the row with the newly created verticies */
	drawRowByVertex(leftV, rightV, y);
	
	free(leftV);
	free(rightV);
	  
    yInc(lEdge);
    yInc(rEdge);
  }
 
  free(lEdge);
  /* now rows from middle-top edge to bottom-top edge */
  lEdge = newEdge(m,t);
  for (; y<=t->y;y++) {
    //drawRow(lEdge->x,rEdge->x,y);
	
	/* Create the left vertex */
	vertex* leftV = (vertex*) malloc(sizeof(vertex));
	leftV->x = lEdge->x;
	leftV->y = y;
	color* nColor = interpolateColorInt(m, t, lEdge->x);
	leftV->v_color = *nColor;
	  
	/* Create the right vertex */
	vertex* rightV = (vertex*) malloc(sizeof(vertex));
	rightV->x = rEdge->x;
	rightV->y = y;
	color* nColor2 = interpolateColorInt(b, t, rEdge->x);
	rightV->v_color = *nColor2;
	  
	/* Draw the row with the newly created verticies */
	drawRowByVertex(leftV, rightV, y);
	
	free(leftV);
	free(rightV);
	  
    yInc(lEdge);
    yInc(rEdge);
  }
  
  free(lEdge);
  free(rEdge);
}
  
/* Draws a triangle with bottom vertex b, top vertex t, and point
   m to the right of line from b to t */
void drawRightTriangle(vertex* b, vertex* t, vertex* m) {
  edge* lEdge;
  edge* rEdge;
  int y;

  /* Draw rows from bottom-top edge to bottom-middle edge */
  lEdge = newEdge(b,t);
  rEdge = newEdge(b,m);

  for (y=b->y; y<m->y; y++) {
    //drawRow(lEdge->x,rEdge->x, y);
	
	  /* Create the left vertex */
	  vertex* leftV = (vertex*) malloc(sizeof(vertex));
	  leftV->x = lEdge->x;
	  leftV->y = y;
	  color* nColor = interpolateColorInt(b, t, lEdge->x);
	  leftV->v_color = *nColor;
	  
	  /* Create the right vertex */
	  vertex* rightV = (vertex*) malloc(sizeof(vertex));
	  rightV->x = rEdge->x;
	  rightV->y = y;
	  color* nColor2 = interpolateColorInt(b, m, rEdge->x);
	  rightV->v_color = *nColor2;
	  
	  /* Draw the row with the newly created verticies */
	  drawRowByVertex(leftV, rightV, y);
	  
	  free(leftV);
	  free(rightV);
	  
    yInc(lEdge);
    yInc(rEdge);
  }
  free(rEdge);

  /* Draw rows from bottom-top edge to middle-top edge */
  rEdge = newEdge(m,t);
  for (; y<=t->y;y++) {
    //drawRow(lEdge->x,rEdge->x, y);
	
	  /* Create the left vertex */
	  vertex* leftV = (vertex*) malloc(sizeof(vertex));
	  leftV->x = lEdge->x;
	  leftV->y = y;
	  color* nColor = interpolateColorInt(b, t, lEdge->x);
	  leftV->v_color = *nColor;
	  
	  /* Create the right vertex */
	  vertex* rightV = (vertex*) malloc(sizeof(vertex));
	  rightV->x = rEdge->x;
	  rightV->y = y;
	  color* nColor2 = interpolateColorInt(m, t, rEdge->x);
	  rightV->v_color = *nColor2;
	  
	  /* Draw the row with the newly created verticies */
	  drawRowByVertex(leftV, rightV, y);
	  
	  free(leftV);
	  free(rightV);
	  
    yInc(lEdge);
    yInc(rEdge);
  }

  free(lEdge);
  free(rEdge);
}
  
/* Main triangle drawing function */
void endTriangle() {

  vertex* b; vertex* t; vertex* m;
  int flag = 0x0; /* bit flags for tests */ 
  int A,B,C; /* implcit line coefficients */
  
  /* First, figure out which vertex should be 
     b (bottom vertex), t (top vertex) and m (middle vertex) */

  if (v[0]->y < v[1]->y) flag = 0x1;
  flag <<= 1; /* shift over one bit */

  if (v[1]->y < v[2]->y) flag |= 0x1;
  flag <<= 1; 

  if (v[2]->y < v[0]->y) flag |= 0x1;

  switch (flag) {

  case 0x1: /* 001 */ 
    t = v[0]; m = v[1]; b = v[2];
    break;

  case 0x2: /* 010 */
    m = v[0]; b = v[1]; t = v[2]; 
    break;

  case 0x3: /* 011 */ 
    t = v[0]; b = v[1]; m = v[2];
    break;

  case 0x4: /* 100 */
    b = v[0]; t = v[1]; m = v[2];
    break;

  case 0x5: /* 101 */
    m = v[0]; t = v[1]; b = v[2]; 
    break;

  case 0x6: /* 110 */
    b = v[0]; m = v[1]; t = v[2]; 
    break;

  default: 
    printf("Bad triangle! \n"); /* only gets here if all vertices equal */
    return;
  }

  /* Now test to see if m is right or left of line bt */
  /* Use implicit line equation */
  A = b->y  -  t->y;
  B = t->x  -  b->x;
  C = b->x * t->y  -  t->x * b->y;

  if (A*m->x + B*m->y + C > 0) 
    drawLeftTriangle(b,t,m);
  else 
    drawRightTriangle(b,t,m);

  free(v[0]);
  free(v[1]);
  free(v[2]);
}



 
