#include "Resources.h"
#include <cassert>

Resources::Resources()
{
	assert(IsWindowReady());	// Window has not been initialized! Check if this is out-of-order
								// TODO Consider throwing if it fails
	alienTexture = LoadTexture("./Assets/Alien.png");
	barrierTexture = LoadTexture("./Assets/Barrier.png");
	shipTextures.push_back(LoadTexture("./Assets/Ship1.png"));
	shipTextures.push_back(LoadTexture("./Assets/Ship2.png"));
	shipTextures.push_back(LoadTexture("./Assets/Ship3.png"));
	laserTexture = LoadTexture("./Assets/Laser.png");
}