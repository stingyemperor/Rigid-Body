#include <math.h>
#include "Plane.h"
#include <Windows.h>
#include <stdio.h>

#define INF 100000.0

Point::Point()
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

Point::Point(float x_, float y_, float z_)
{
	x = x_;
	y = y_;
	z = z_;
}


Plane::Plane()
{
	a = 0.0;
	b = 0.0;
	c = 0.0;
	d = 0.0;
}

Plane::Plane(Point p1, Point p2, Point p3)
{
	float a1 = p2.x - p1.x;
	float b1 = p2.y - p1.y;
	float c1 = p2.z - p1.z;
	float a2 = p3.x - p1.x;
	float b2 = p3.y - p1.y;
	float c2 = p3.z - p1.z;
	a = b1 * c2 - b2 * c1;
	b = a2 * c1 - a1 * c2;
	c = a1 * b2 - b1 * a2;
	d = (-a * p1.x - b * p1.y - c * p1.z);
	float norm = sqrt(a*a + b*b + c*c);
	a = a / norm; b = b / norm; c = c / norm; d = d / norm;
}


bool onSegment(Point p, Point q, Point r)
{
	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
		return true;
	return false;
}

int orientation(Point p, Point q, Point r)
{
	float val = (q.y - p.y) * (r.x - p.x) -
		(q.x - p.x) * (r.y - p.y);

	if (fabs(val) < 1e-6) return 0;  // colinear 
	return (val > 0) ? 1 : 2; // clock or counterclock wise 
}

bool doIntersect(Point p1, Point q1, Point p2, Point q2)
{
	// Find the four orientations needed for general and special cases 
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case 
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases 
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases 
}

bool isInside(Point polygon[], int n, Point p)
{
	// There must be at least 3 vertices in polygon[] 
	if (n < 3)  return false;

	// Create a point for line segment from p to infinite 
	Point extreme(INF, p.y);

	// Count intersections of the above line with sides of polygon 
	int count = 0, i = 0;
	do
	{
		int next = (i + 1) % n;

		if (doIntersect(polygon[i], polygon[next], p, extreme))
		{
			// If the point 'p' is colinear with line segment 'i-next', 
			// then check if it lies on segment. If it lies, return true, 
			// otherwise false 
			
			if (orientation(polygon[i], p, polygon[next]) == 0)
				return onSegment(polygon[i], p, polygon[next]);

			count++;
		}
		i = next;
	} while (i != 0);

	// Return true if count is odd, false otherwise 
	return count & 1;  // Same as (count%2 == 1) 
}

bool isInside_yz(Point polygon[], int n, Point p)
{
	// There must be at least 3 vertices in polygon[] 
	if (n < 3)  return false;

	// Create a point for line segment from p to infinite 
	Point extreme(INF, p.z), new_p,new_pn, point_new;

	// Count intersections of the above line with sides of polygon 
	int count = 0, i = 0;
	do
	{
		int next = (i + 1) % n;
		new_p.x = polygon[i].y; new_pn.x = polygon[next].y;
		new_p.y = polygon[i].z; new_p.z = polygon[i].x;
		new_pn.y = polygon[next].z; new_pn.z = polygon[next].x; 
		point_new.x = p.x;
		point_new.y = p.z;
		point_new.z = p.x;
		if (doIntersect(new_p, new_pn, point_new, extreme))
		{
			// If the point 'p' is colinear with line segment 'i-next', 
			// then check if it lies on segment. If it lies, return true, 
			// otherwise false 
			if (orientation(new_p, p, new_pn) == 0)
				return onSegment(new_p, p, new_pn);

			count++;
		}
		i = next;
	} while (i != 0);

	
	// Return true if count is odd, false otherwise 
	return count & 1;  // Same as (count%2 == 1) 
}

bool isInside_xz(Point polygon[], int n, Point p)
{
	// There must be at least 3 vertices in polygon[] 
	if (n < 3)  return false;

	// Create a point for line segment from p to infinite 
	Point extreme(INF, p.z), new_p, new_pn, point_new;

	// Count intersections of the above line with sides of polygon 
	int count = 0, i = 0;
	do
	{
		int next = (i + 1) % n;
		new_p.x = polygon[i].x; new_pn.x = polygon[next].x;
		new_p.y = polygon[i].z; new_p.z = polygon[i].x;
		new_pn.y = polygon[next].z; new_pn.z = polygon[next].x;
		point_new.x = p.x;
		point_new.y = p.z;
		point_new.z = p.x;
		if (doIntersect(new_p, new_pn, point_new, extreme))
		{
			// If the point 'p' is colinear with line segment 'i-next', 
			// then check if it lies on segment. If it lies, return true, 
			// otherwise false 
			if (orientation(new_p, p, new_pn) == 0)
				return onSegment(new_p, p, new_pn);

			count++;
		}
		i = next;
	} while (i != 0);


	// Return true if count is odd, false otherwise 
	return count & 1;  // Same as (count%2 == 1) 
}