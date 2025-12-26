#pragma once
#include "raylib.h"
#include "raymath.h"	


inline bool pointInCircle(Vector2 circlePos, float radius, Vector2 point)	// TODO Rename to "isPointInCircle" to better reflect usage
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