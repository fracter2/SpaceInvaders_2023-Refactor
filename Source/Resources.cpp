#include <cassert>
#include <stdexcept>

#include "Resources.h"
#include "gsl/assert"

Resources::Resources()
{
	if (!IsWindowReady()) {
		throw std::runtime_error("Window is not ready! Resources cannot load before this!");
	}

	alienTexture = LoadTexture("./Assets/Alien.png");
	barrierTexture = LoadTexture("./Assets/Barrier.png");
	laserTexture = LoadTexture("./Assets/Laser.png");

	// NOTE We do not need bounds-checking because each file/index is selected manually
	GSL_SUPPRESS(bounds.4) shipTextures[0] = LoadTexture("./Assets/Ship1.png");
	GSL_SUPPRESS(bounds.4) shipTextures[1] = LoadTexture("./Assets/Ship2.png");
	GSL_SUPPRESS(bounds.4) shipTextures[2] = LoadTexture("./Assets/Ship3.png");
}

Resources::~Resources()
{
	UnloadTexture(alienTexture);
	UnloadTexture(barrierTexture);
	UnloadTexture(laserTexture);
	GSL_SUPPRESS(bounds.4) UnloadTexture(shipTextures[0]);
	GSL_SUPPRESS(bounds.4) UnloadTexture(shipTextures[1]);
	GSL_SUPPRESS(bounds.4) UnloadTexture(shipTextures[2]);
}
