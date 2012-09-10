/*****************************
 4 x 4 Matrix class
 Implemented by Jordan Ell
 Copyright 2012
 ******************************/
 
#include "Matrix4.h"
#include "Vector4.h"

#include <iostream>

using namespace std;
 
Matrix4::Matrix4()
{
	float identity[16] = {1,0,0,0,
						  0,1,0,0,
						  0,0,1,0,
						  0,0,0,1};
						  
	for(int i = 0; i < 16; i++)
		this->data[i] = identity[i];
}
	 
Matrix4::Matrix4(const Matrix4 &copied)
{
	 for(int i = 0; i < 16; i++)
		 this->data[i] = copied.data[i];
}
	 
Matrix4::~Matrix4()
{
	// Nothing outside of class to destruct.
}

void Matrix4::print()
{
	std::cout << "{" << data[0] << "," << data[1] << "," << data[2] << "," << data[3] << "\n";
	std::cout << " " << data[4] << "," << data[5] << "," << data[6] << "," << data[7] << "\n";
	std::cout << " " << data[8] << "," << data[9] << "," << data[10] << "," << data[11] << "\n";
	std::cout << " " << data[12] << "," << data[13] << "," << data[14] << "," << data[15] << "}\n";
}

void Matrix4::identity()
{
	float identity[16] = {1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1};
	
	for(int i = 0; i < 16; i++)
		this->data[i] = identity[i];
}

void Matrix4::transpose()
{
	float temp[16];
	
	for(int i = 0; i < 16; i++)
		temp[i] = this->data[i];
	
	// First row
	this->data[1] = temp[4];
	this->data[2] = temp[8];
	this->data[3] = temp[12];
	
	// Second row
	this->data[4] = temp[1];
	this->data[6] = temp[9];
	this->data[7] = temp[13];
	
	// Third row
	this->data[8] = temp[2];
	this->data[9] = temp[6];
	this->data[11] = temp[14];
	
	// Fourth row
	this->data[12] = temp[3];
	this->data[13] = temp[7];
	this->data[14] = temp[11];
}

void Matrix4::zero()
{
	for(int i = 0; i < 16; i++)
		this->data[i] = 0;
}

float Matrix4::getValue(int i, int j)
{
	int index = (i-1)*4 + (j-1);
	return data[index];
}

void Matrix4::turnLeft(float angle)
{
	data[0] = cos(angle);
	data[1] = -sin(angle);
	data[4] = sin(angle);
	data[5] = cos(angle);
}

void Matrix4::turnRight(float angle)
{
	turnLeft(-angle);
}

Vector4 Matrix4::getTranslation()
{
	Vector4 newVector4;
	
	newVector4.set(data[3],data[7],data[11],0);
	
	return newVector4;
}

void Matrix4::setTranslation(const Vector4 &v)
{
	data[3] = v.data[0];
	data[7] = v.data[1];
	data[11] = v.data[2];
}

void Matrix4::setTranslation(const float &a, const float &b, const float &c, const float&d)
{
	data[3] = a;
	data[7] = b;
	data[11] = c;
	data[15] = d;
}

void Matrix4::setScaleX(float x)
{
	data[0] = x;
}

void Matrix4::setScaleY(float y)
{
	data[5] = y;
}

void Matrix4::setScaleZ(float z)
{
	data[10] = z;
}

float Matrix4::operator() (float i, float j)
{
	int index = (i-1)*4 + (j-1);
	return data[index];
}

Matrix4 Matrix4::operator=(const Matrix4 &other)
{
	for(int i = 0; i < 16; i++)
		this->data[i] = other.data[i];
}

bool Matrix4::operator==(const Matrix4 &other) const
{
	for(int i = 0; i < 16; i++)
	{
		if(this->data[i] != other.data[i])
			return false;
	}
	
	return true;
}

bool Matrix4::operator!=(const Matrix4 &other) const
{
	for(int i = 0; i < 16; i++)
	{
		if(this->data[i] != other.data[i])
			return false;
	}
	
	return true;
}

Matrix4 Matrix4::operator*=(const float &k)
{
	for(int i = 0; i < 16; i++)
		this->data[i] *= k;
}

Matrix4 operator*(const Matrix4 &M, const float &K)
{
	Matrix4 newMatrix4;
	
	for(int i = 0; i < 16; i++)
		newMatrix4.data[i] = M.data[i]*K;
		
	return newMatrix4;
}

Matrix4 operator*(const float &K, const Matrix4 &M)
{
	Matrix4 newMatrix4;
	
	for(int i = 0; i < 16; i++)
		newMatrix4.data[i] = M.data[i]*K;
	
	return newMatrix4;
}

Matrix4 operator*(Matrix4 lhm, Matrix4 rhm)
{
	Matrix4 newMatrix4;
	
	int i, j;
	for(int a = 0; a < 16; a++)
	{
		i = a/4 + 1;
		j = a%4 + 1;
		//cout << "i: " << i << "\n";
		//cout << "j: " << j << "\n";
		//cout << lhm(i,1) << "*" << rhm(1,j) << "+" << lhm(i,2) << "*" << rhm(2,j) << "+" << lhm(i,3) << "*" << rhm(3,j) << "+" << lhm(i,4) << "*" << rhm(4,j) << "\n";
		newMatrix4.data[a] = lhm(i,1)*rhm(1,j) + lhm(i,2)*rhm(2,j) + lhm(i,3)*rhm(3,j) + lhm(i,4)*rhm(4,j);
	}
	
	return newMatrix4;
}