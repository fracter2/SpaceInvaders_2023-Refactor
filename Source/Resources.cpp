#include <cassert>

#include "Resources.h"

Resources::Resources()
{
	assert(IsWindowReady());	// Window has not been initialized! Check if this is out-of-order
								// TODO Consider throwing if it fails
	alienTexture = LoadTexture("./Assets/Alien.png");
	barrierTexture = LoadTexture("./Assets/Barrier.png");
	laserTexture = LoadTexture("./Assets/Laser.png");

	shipTextures[0] = LoadTexture("./Assets/Ship1.png");
	shipTextures[1] = LoadTexture("./Assets/Ship2.png");
	shipTextures[2] = LoadTexture("./Assets/Ship3.png");
}