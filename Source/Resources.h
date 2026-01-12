#pragma once 		// TODO Consider making into #ifdef... type include guard (portability, recomended by cpp guidelines)
#include <array>

#include "raylib.h"

struct Resources
{
	Resources();

	static constexpr size_t shipTextureCount = 3;
	std::array<Texture2D, shipTextureCount> shipTextures;	// TODO Does it need to be a vector? is the size known at compile-time? PROBABLY, convert to std::array or equivolent to better reflect usage
	Texture2D alienTexture;
	Texture2D barrierTexture;
	Texture2D laserTexture;
};