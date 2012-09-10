/*********
 Vector Class
 Written by Jordan Ell
 Copyright 2012
 *********/
 
#include "Vector4.h"
 
Vector4::Vector4()
{
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;
}

Vector4::Vector4(const float& a, const float& b, const float& c, const float& d)
{
	data[0] = a;
	data[1] = b;
	data[2] = c;
	data[3] = d;
}

Vector4::Vector4(const Vector4& copied)
{
	data[0] = copied.data[0];
	data[1] = copied.data[1];
	data[2] = copied.data[2];
	data[3] = copied.data[3];
}

Vector4::~Vector4()
{
	// Nothing extra to destruct
}

void Vector4::set(float a, float b, float c, float d)
{
	data[0] = a;
	data[1] = b;
	data[2] = c;
	data[3] = d;
}

float Vector4::norm()
{
	return sqrt((data[0] * data[0]) + (data[1] * data[1]) + (data[2] * data[2]));
}

void Vector4::normalize()
{
	float norm = this->norm();
	data[0] = data[0]/norm;
	data[1] = data[1]/norm;
	data[2] = data[2]/norm;
	data[3] = 0;
}

void Vector4::zero()
{
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;
}

void Vector4::print()
{
	std::cout << "{" << data[0] << "," << data[1] << "," << data[2] << "," << data[3] << "}\n";
}

Vector4 Vector4::operator = (const Vector4 &other)
{
	data[0] = other.data[0];
	data[1] = other.data[1];
	data[2] = other.data[2];
	data[3] = other.data[3];
}

Vector4 Vector4::operator + (const Vector4 &other)
{
	Vector4 newVector4;
	
	newVector4.set(
				   data[0] + other.data[0],
				   data[1] + other.data[1],
				   data[2] + other.data[2],
				   0.0f);
				   
	return newVector4;
}

Vector4 Vector4::operator - (const Vector4 &other)
{
	Vector4 newVector4;
	
	newVector4.set(
				   data[0] - other.data[0],
				   data[1] - other.data[1],
				   data[2] - other.data[2],
				   0);
	
	return newVector4;
}

void Vector4::operator += (const Vector4 &other)
{
	data[0] += other.data[0];
	data[1] += other.data[1];
	data[2] += other.data[2];
}

void Vector4::operator -= (const Vector4 &other)
{
	data[0] -= other.data[0];
	data[1] -= other.data[1];
	data[2] -= other.data[2];
}

void Vector4::operator *= (const float &scalar)
{
	data[0] *= scalar;
	data[1] *= scalar;
	data[2] *= scalar;
}

bool Vector4::operator == (const Vector4 &other)
{
	if(data[0] == other.data[0] && data[1] == other.data[1]
	   && data[2] == other.data[2] && data[3] == other.data[3])
		return true;
	else 
		return false;
}

bool Vector4::operator != (const Vector4 &other)
{
	if(data[0] == other.data[0] && data[1] == other.data[1]
	   && data[2] == other.data[2] && data[3] == other.data[3])
		return false;
	else 
		return true;
}

Vector4 Vector4::operator *(const float &s)
{
	Vector4 newVector4;
	
	newVector4.set(
				   data[0]*s,
				   data[1]*s,
				   data[2]*s,
				   0);
	
	return newVector4;
}

float Vector4::operator *(const Vector4 &RHV)
{
	float value = 0;
	
	value = data[0]*RHV.data[0] + data[1]*RHV.data[1] + data[2]*RHV.data[2];
	
	return value;
}

Vector4 Vector4::operator ^(const Vector4 &RHV)
{
	Vector4 newVector4;
	
	newVector4.set(
				   data[1]*RHV.data[2] - data[2]*RHV.data[1],
				   data[2]*RHV.data[0] - data[0]*RHV.data[2],
				   data[0]*RHV.data[1] - data[1]*RHV.data[0],
				   0);
	
	return newVector4;
}