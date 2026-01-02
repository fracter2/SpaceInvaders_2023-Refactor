#pragma once
#include "raylib.h"
#include "raymath.h"

inline bool isPointInCircle(Vector2 circlePos, float radius, Vector2 point)	// TODO Rename to "isPointInCircle" to better reflect usage
{
	float distanceToCentre = Vector2Distance(circlePos, point);	// TODO Remove, redundant

	if (distanceToCentre < radius)
	{
		return true;
	}
	else
	{
		return false;
	}
}


inline bool CheckCollision(Vector2 circlePos, float circleRadius, Vector2 lineStart, Vector2 lineEnd)
{
	// our objective is to calculate the distance between the closest point on the line to the centre of the circle, 
	// and determine if it is shorter than the radius.

	// check if either edge of line is within circle
	if (isPointInCircle(circlePos, circleRadius, lineStart) || isPointInCircle(circlePos, circleRadius, lineEnd))
	{
		return true;
	}

	// simplify variables		// TODO consider renaming to follow variable naming convention
	Vector2 A = lineStart;
	Vector2 B = lineEnd;
	Vector2 C = circlePos;

	// calculate the length of the line
	float length = Vector2Distance(A, B);

	// calculate the dot product
	float dotP = (((C.x - A.x) * (B.x - A.x)) + ((C.y - A.y) * (B.y - A.y))) / pow(length, 2);		// TODO Make separate func

	// use dot product to find closest point
	float closestX = A.x + (dotP * (B.x - A.x));
	float closestY = A.y + (dotP * (B.y - A.y));

	//find out if coordinates are on the line.
	// we do this by comparing the distance of the dot to the edges, with two vectors
	// if the distance of the vectors combined is the same as the length the point is on the line

	//since we are using floating points, we will allow the distance to be slightly innaccurate to create a smoother collision
	float buffer = 0.1;																				// TODO Make static constexpr

	float closeToStart = Vector2Distance(A, { closestX, closestY }); //closestX + Y compared to line Start
	float closeToEnd = Vector2Distance(B, { closestX, closestY });	//closestX + Y compared to line End

	float closestLength = closeToStart + closeToEnd;

	if (closestLength == length + buffer || closestLength == length - buffer)						// BUG Shouldn't this be <= and >= ?
	{
		//Point is on the line!

		//Compare length between closest point and circle centre with circle radius

		float closeToCentre = Vector2Distance(A, { closestX, closestY }); //closestX + Y compared to circle centre

		if (closeToCentre < circleRadius)
		{
			//Line is colliding with circle!
			return true;
		}
		else
		{
			//Line is not colliding
			return false;
		}
	}
	else
	{
		// Point is not on the line, line is not colliding
		return false;
	}

}