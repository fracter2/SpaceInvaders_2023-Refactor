#include <cassert>

#include "Resources.h"
#include "gsl/assert"

Resources::Resources()
{
	assert(IsWindowReady());	// Window has not been initialized! Check if this is out-of-order
								// TODO Consider throwing if it fails
	alienTexture = LoadTexture("./Assets/Alien.png");
	barrierTexture = LoadTexture("./Assets/Barrier.png");
	laserTexture = LoadTexture("./Assets/Laser.png");

	// NOTE We do not need bounds-checking because each file/index is selected manually
	GSL_SUPPRESS(bounds.4) shipTextures[0] = LoadTexture("./Assets/Ship1.png");
	GSL_SUPPRESS(bounds.4) shipTextures[1] = LoadTexture("./Assets/Ship2.png");
	GSL_SUPPRESS(bounds.4) shipTextures[2] = LoadTexture("./Assets/Ship3.png");
}