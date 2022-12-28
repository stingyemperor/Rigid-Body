#pragma once
#include <math.h>
#include <cmath>

class Point {
public:

	Point();
	Point(float x_, float y_, float z_= 0.0);

	float x;
	float y;
	float z;
};

class Plane{
public:

	float a, b, c, d;
	Plane();
	Plane(Point p1, Point p2, Point p3);
};


bool onSegment(Point p, Point q, Point r);
int orientation(Point p, Point q, Point r);
bool doIntersect(Point p1, Point q1, Point p2, Point q2);
bool isInside(Point polygon[], int n, Point p);
bool isInside_yz(Point polygon[], int n, Point p);
bool isInside_xz(Point polygon[], int n, Point p);
