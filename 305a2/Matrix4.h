/*****************************
   4 x 4 Matrix class
   Implemented by Jordan Ell
   Copyright 2012
******************************/


#ifndef MATRIX4_H
#define MATRIX4_H

#include "Vector4.h"
#include <iostream>

class Matrix4 {

 public:
  float data[16];  //opengl style matrix

  Matrix4();
  Matrix4(const Matrix4 &copied);
  ~Matrix4();

  void print();
  void identity();
  void transpose();
  void zero();
  float getValue(int i, int j);

  void turnLeft(float angle);
  void turnRight(float angle);

  Vector4 getTranslation();
  void setTranslation(const Vector4 &v);
  void setTranslation(const float &a, const float &b,
		      const float &c, const float&d);

                		
  // operator overloading
  
  float operator() (float i, float j);
  
  Matrix4 operator=(const Matrix4 &other);
  
  bool operator==(const Matrix4 &other) const;
  bool operator!=(const Matrix4 &other) const;
  
  Matrix4 operator*=(const float &k);
  
  friend Matrix4 operator*(const Matrix4 &M, const float &K);
  friend Matrix4 operator*(const float &K, const Matrix4 &M);
  friend Matrix4 operator*(Matrix4 lhm, Matrix4 rhm);
  
}; //end of class Matrix4


#endif
