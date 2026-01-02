// TODO Rename file to be un-capitalized, including the .cpp

#pragma once
#include "raylib.h"
#include "vector"		// TODO Make to use <> like everywhere else

struct Resources
{
	Resources();
	//void Unload();						// TODO remove, unused

	std::vector<Texture2D> shipTextures;	// Does it need to be a vector? is the size known at compile-time? PROBABLY, convert to std::array or equivolent to better reflect usage
	Texture2D alienTexture;
	Texture2D barrierTexture;
	Texture2D laserTexture;

};