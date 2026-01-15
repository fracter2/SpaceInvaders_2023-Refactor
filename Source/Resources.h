#pragma once 		// TODO Consider making into #ifdef... type include guard (portability, recomended by cpp guidelines)
#include <array>

#include "raylib.h"

struct Resources {
	Resources();
	~Resources();

	Resources(const Resources&) = delete;
	Resources& operator=(const Resources&) = delete;
	Resources(Resources&&) = delete;
	Resources& operator=(Resources&&) = delete;

	static constexpr size_t shipTextureCount = 3;
	std::array<Texture2D, shipTextureCount> shipTextures;
	Texture2D alienTexture;
	Texture2D barrierTexture;
	Texture2D laserTexture;
};