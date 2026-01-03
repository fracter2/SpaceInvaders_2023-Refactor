#include "game.h"
#include <iostream>
#include <vector>
#include "common.h"


Game::Game(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb, Resources& res) noexcept
	: transitionTo(transitionFunc)
	, leaderboard(&lb)
	, resources(&res)
	, background(Background(600))		// TODO Clarify magic number
{

	float wallDistance = (float)GetScreenWidth() / (float)(wallCount + 1);
	for (int i = 0; i < wallCount; i++)
	{
		Vector2 pos = { 
			wallDistance* (i + 1),
			(float)GetScreenHeight() - 250
		};
		Walls.push_back(Wall(pos));
	}

	SpawnAliens();

	leaderboard->currentScore = 0;
}

void Game::Update() noexcept
{
	if (IsKeyReleased(KEY_Q))
	{
		transitionTo(SceneId::EndScreen);
		return;
	}

	player.Update();
		
	for (int i = 0; i < Aliens.size(); i++)		// TODO make into a for each loop
	{
		Aliens[i].Update(); 

		if (Aliens[i].position.y > player.position.y)
		{
			transitionTo(SceneId::EndScreen);
			return;
		}
	}

	if (player.lives < 1)						// TODO Shouldn't this go after projectile update?
	{
		transitionTo(SceneId::EndScreen);
		return;
	}

	if (Aliens.size() < 1)						// TODO Use .empty() for clarity
	{
		SpawnAliens();
	}

	background.offset = abs(player.position.x) / 15;	// TODO Clarify 15 as offset-multiplier

	for (int i = 0; i < Projectiles.size(); i++)		// TODO Make foreach loop
	{
		Projectiles[i].Update();
	}
	for (int i = 0; i < Walls.size(); i++)				// TODO Make foreach loop
	{
		Walls[i].Update();
	}

	//CHECK ALL COLLISONS HERE
	for (int i = 0; i < Projectiles.size(); i++)
	{
		if (Projectiles[i].fromPlayer)
		{
			for (int a = 0; a < Aliens.size(); a++)
			{
				if (CheckCollisionCircleLine(Aliens[a].position, Aliens[a].radius, Projectiles[i].getLineStart(), Projectiles[i].getLineEnd()))
				{
					// Kill!
					std::cout << "Hit! \n";
					Projectiles[i].active = false;
					Aliens[a].active = false;
					leaderboard->currentScore += 100;
				}
			}
		}

		//ENEMY PROJECTILES HERE
		for (int i = 0; i < Projectiles.size(); i++)		// TODO Merge with other projectile loop
		{
			if (!Projectiles[i].fromPlayer)
			{
				if (CheckCollisionCircleLine({player.position.x, player.position.y }, player.radius, Projectiles[i].getLineStart(), Projectiles[i].getLineEnd()))
				{
					std::cout << "dead!\n"; 
					Projectiles[i].active = false; 
					player.lives -= 1; 
				}
			}
		}


		for (int b = 0; b < Walls.size(); b++)
		{
			if (CheckCollisionCircleLine(Walls[b].position, Walls[b].radius, Projectiles[i].getLineStart(), Projectiles[i].getLineEnd()))
			{
				// Kill!
				std::cout << "Hit! \n";
				Projectiles[i].active = false;
				Walls[b].health -= 1;
			}
		}
	}

	if (IsKeyPressed(KEY_SPACE))
	{
		Projectiles.push_back(
			Projectile(player.position, { 0, -1 }, true)
		);
	}

	shootTimer += 1;												// TODO Refactor away using GetTime() and modulo
	if (shootTimer > 59) //once per second
	{
		shootTimer = 0;
		int randomAlienIndex = 0;

		if (Aliens.size() > 1)										// TODO Remove, redundant check since e're using index 0 either way
		{
			randomAlienIndex = rand() % Aliens.size();
		}

		Projectiles.push_back(
			Projectile(Aliens[randomAlienIndex].position, { 0, 1 }, false)
		);
	}

	// REMOVE INACTIVE/DEAD ENITITIES
	for (int i = 0; i < Projectiles.size(); i++)
	{
		if (Projectiles[i].active == false)				// TODO Check if there's an algorithm for this (there always is)
		{
			Projectiles.erase(Projectiles.begin() + i);
			// Prevent the loop from skipping an instance because of index changes, since all insances after
			// the killed objects are moved down in index. This is the same for all loops with similar function
			i--;
		}
	}
	for (int i = 0; i < Aliens.size(); i++)
	{
		if (Aliens[i].active == false)					// TODO Check if there's an algorithm for this (there always is)
		{
			Aliens.erase(Aliens.begin() + i);
			i--;
		}
	}
	for (int i = 0; i < Walls.size(); i++)
	{
		if (Walls[i].active == false)					// TODO Check if there's an algorithm for this (there always is)
		{
			Walls.erase(Walls.begin() + i);
			i--;
		}
	}
}


void Game::Render() const noexcept
{
	background.Render();

	DrawText(TextFormat("Score: %i", leaderboard->currentScore), 50, 20, 40, YELLOW);
	DrawText(TextFormat("Lives: %i", player.lives), 50, 70, 40, YELLOW);

	player.Render(*resources);

	for (int i = 0; i < Projectiles.size(); i++)
	{
		Projectiles[i].Render(*resources);
	}

	for (int i = 0; i < Walls.size(); i++)
	{
		Walls[i].Render(*resources);
	}

	for (int i = 0; i < Aliens.size(); i++)
	{
		Aliens[i].Render(*resources);
	}
}

void Game::SpawnAliens()
{
	static constexpr int formationWidth = 8;
	static constexpr int formationHeight = 5;
	static constexpr int alienSpacing = 80;
	static constexpr int formationX = 550;
	static constexpr int formationY = 50;

	for (int row = 0; row < formationHeight; row++) {
		for (int col = 0; col < formationWidth; col++) {
			Vector2 pos = {
				formationX + (col * alienSpacing),
				formationY + (row * alienSpacing)
			};
			Aliens.push_back(Alien(pos));
		}
	}
}

Player::Player() noexcept
{
	position = Vector2(
		(float)GetScreenWidth() / 2,
		(float)(GetScreenHeight() - player_y_offset)
	);
	std::cout<< "Find Player -X:" << position.x << "Find Player -Y" << position.y << std::endl;	// TODO Remove, redudant printing

}

void Player::Update() 
{
	//Movement
	int direction = IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT);
	position.x += speed * direction;

	// Border collision
	position.x = Clamp(position.x, 0 + radius, GetScreenWidth() - radius);
	
}

void Player::Render(const Resources& res) const noexcept
{
	static constexpr float timePerFrame = 0.4;
	auto frames = res.shipTextures;
	// TODO Consider moving some / most / all these args into static constexpr variables
	DrawTexturePro(frames[(int)(GetTime() / timePerFrame) % frames.size()],						// TODO Make sure this is noexcept
		{
			0,
			0,
			352,
			352,
		},
		{
			position.x, 
			position.y,
			100,
			100,
		}, { 50, 50 },
		0,
		WHITE);
}

Projectile::Projectile(Vector2 pos, Vector2 direction, bool fromPlayer) noexcept
	: position(pos)
	, direction(direction)
	, fromPlayer(fromPlayer)
{
}

void Projectile::Update()
{
	position = Vector2Add(position, direction * speed);

	if (position.y < 0 || position.y > 1500)		// TODO Clarify magic numbers
	{
		active = false;
	}
}

void Projectile::Render(const Resources& res) const noexcept
{
	// TODO Consider moving some / most / all these args into static constexpr variables
	DrawTexturePro(res.laserTexture,
		{
			0,
			0,
			176,
			176,
		},
		{
			position.x,
			position.y,
			50,
			50,
		}, { 25 , 25 },
		0,
		WHITE);
}

Wall::Wall(Vector2 pos) noexcept 
	:position(pos)
{
}

void Wall::Render(const Resources& res) const noexcept			// TODO Make Texture2D&
{
	// TODO Consider moving some / most / all these args into static constexpr variables
	DrawTexturePro(res.barrierTexture,
		{
			0,
			0,
			704,
			704,
		},
		{
			position.x,
			position.y,
			200,
			200,
		}, { 100 , 100 },
		0,
		WHITE);

	// TODO Consider moving some / most / all these args into static constexpr variables
	DrawText(TextFormat("%i", health), position.x-21, position.y+10, 40, RED);		// TODO Consider using modern std::format
	
}

void Wall::Update() 
{
	if (health < 1)
	{
		active = false;
	}


}

Alien::Alien(Vector2 pos) noexcept
	: position(pos)
{
}

void Alien::Update() 
{
	if (moveRight)					// TODO Move statement to just the position.x update, perhaps as ternary operator
	{
		position.x += speed; 

		if (position.x >= GetScreenWidth())
		{
			moveRight = false;		// TODO Consider making into an int, like with player direction
			position.y += 50;		// TODO Make into static constexpr
		}
	}
	else 
	{
		position.x -= speed; 

		if (position.x <= 0)
		{
			moveRight = true; 
			position.y += 50; 
		}
	}
}

void Alien::Render(const Resources& res) const noexcept
{		
	// TODO Consider moving some / most / all these args into static constexpr variables
	DrawTexturePro(res.alienTexture,
		{
			0,
			0,
			352,
			352,
		},
		{
			position.x,
			position.y,
			100,
			100,
		}, {50 , 50},
		0,
		WHITE);
}


//BACKGROUND

Star::Star(Vector2 pos, float size) noexcept
	: position(pos)
	, size(size)
{
}

void Star::Render(float offset) const noexcept
{
	DrawCircle((int)position.x + offset, (int)position.y, size, color);	// TODO Make sure that this is noexcept
}


Background::Background(int starAmount)
{
	for (int i = 0; i < starAmount; i++)
	{
		Vector2 pos = { 
			GetRandomValue(-150, GetScreenWidth() + 150), 
			GetRandomValue(0, GetScreenHeight())
		};

		float size = GetRandomValue(1, 4) / 2;

		Stars.push_back(Star(pos, size));
	}
}

void Background::Render() const noexcept
{
	for (int i = 0; i < Stars.size(); i++)
	{
		Stars[i].Render(offset);
	}
}