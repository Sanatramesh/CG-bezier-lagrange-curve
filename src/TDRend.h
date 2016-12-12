#ifndef TDREND_H
#define TDREND_H

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <GL/gl.h>
#include <math.h>

extern int h, w;
extern float transx, transy, transz;
extern float scale;

void lagrange_curve();
void bezier_curve();
void drawLagrangeCurve();
void drawBezierCurve();
void drawAxis();
void calculateCentroid();

float minx();
float maxx();

class Point{
	float x, y, z;
public:
	Point();
	Point(float x_coor, float y_coor, float z_coor);
	float getX();
	float getY();
	float getZ();
	void setX(float x_coor);
	void setY(float y_coor);
	void setZ(float z_coor);
};

#endif
