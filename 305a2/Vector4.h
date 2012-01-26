/*********
 Vector Class
Written by Jordan Ell
Copyright 2012
*********/


#ifndef VECTOR4_H
#define VECTOR4_H

#include "math.h"
#include <iostream>

class Vector4 {
 public:
  float data[4];

  Vector4();
  Vector4(const float& a, const float& b, const float& c, const float& d);
  Vector4(const Vector4& copied);
  ~Vector4();

  void set(float a, float b, float c, float d);
  float norm(); //returns length of vector
  void normalize(); //makes vector unit length
  void zero(); //assigns zeros to all vector values
  void print();

  float x(){return data[0];}
  float y(){return data[1];}
  float z(){return data[2];}
  float w(){return data[3];}

  void x(const float& val){data[0]=val;}
  void y(const float& val){data[1]=val;}
  void z(const float& val){data[2]=val;}
  void w(const float& val){data[3]=val;}

  Vector4 operator = (const Vector4 &other);
  Vector4 operator + (const Vector4 &other);
  Vector4 operator - (const Vector4 &other);

  void operator += (const Vector4 &other);
  void operator -= (const Vector4 &other);
  void operator *= (const float &scalar);

  bool operator == (const Vector4 &other);
  bool operator != (const Vector4 &other);

  friend Vector4 operator *(const float &s, const Vector4 &v);
  friend Vector4 operator *(const Vector4 &v, const float &s);
  
  friend Vector4 cross_product(const Vector4 &LHV, const Vector4 &RHV); 
  friend float dot_product(const Vector4 &LHV, const Vector4 &RHV);
};

#endif  

