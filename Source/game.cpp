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

	float window_width = (float)GetScreenWidth();		// TODO Move to where they are used, since their only used once
	float window_height = (float)GetScreenHeight();		// TODO Move to where they are used, since their only used once
	float wall_distance = window_width / (wallCount + 1); 
	for (int i = 0; i < wallCount; i++)
	{
		Wall newWalls;									// TODO Refactor away 2-step init with the position
		newWalls.position.y = window_height - 250; 
		newWalls.position.x = wall_distance * (i + 1); 

		Walls.push_back(newWalls); 

	}

	SpawnAliens();

	leaderboard->currentScore = 0;
}

void Game::Update() noexcept
{
	if (IsKeyReleased(KEY_Q))
	{
		transitionTo(SceneId::EndScreen);
	}

	player.Update();
		
	for (int i = 0; i < Aliens.size(); i++)		// TODO make into a for each loop
	{
		Aliens[i].Update(); 

		if (Aliens[i].position.y > player.position.y)
		{
			transitionTo(SceneId::EndScreen);
		}
	}

	if (player.lives < 1)						// TODO Shouldn't this go after projectile update?
	{
		transitionTo(SceneId::EndScreen);
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
				if (CheckCollision(Aliens[a].position, Aliens[a].radius, Projectiles[i].lineStart, Projectiles[i].lineEnd))
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
				if (CheckCollision({player.position.x, player.position.y }, player.radius, Projectiles[i].lineStart, Projectiles[i].lineEnd))
				{
					std::cout << "dead!\n"; 
					Projectiles[i].active = false; 
					player.lives -= 1; 
				}
			}
		}


		for (int b = 0; b < Walls.size(); b++)
		{
			if (CheckCollision(Walls[b].position, Walls[b].radius, Projectiles[i].lineStart, Projectiles[i].lineEnd))
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
		float window_height = (float)GetScreenHeight();
		Projectile newProjectile;									// TODO Fix multiple-step initialization
		newProjectile.position.x = player.position.x;
		newProjectile.position.y = window_height - 130;
		newProjectile.fromPlayer = true;
		Projectiles.push_back(newProjectile);
	}

	shootTimer += 1;												// TODO Refactor away using GetTime() and modulo
	if (shootTimer > 59) //once per second
	{
		int randomAlienIndex = 0;

		if (Aliens.size() > 1)
		{
			randomAlienIndex = rand() % Aliens.size();
		}

		Projectile newProjectile;
		newProjectile.position = Aliens[randomAlienIndex].position;
		newProjectile.position.y += 40;
		newProjectile.speed = -15;
		newProjectile.fromPlayer = false;
		Projectiles.push_back(newProjectile);
		shootTimer = 0;
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



// TODO Move to separate file, this does not depend on Game:: and is just math with the args
bool Game::CheckCollision(Vector2 circlePos, float circleRadius, Vector2 lineStart, Vector2 lineEnd) 
{
	// our objective is to calculate the distance between the closest point on the line to the centre of the circle, 
	// and determine if it is shorter than the radius.

	// check if either edge of line is within circle
	if (pointInCircle(circlePos, circleRadius, lineStart) || pointInCircle(circlePos, circleRadius, lineEnd))
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



void Projectile::Update()
{
	position.y -= speed;

	// UPDATE LINE POSITION							// TODO Remove this block, this is just the position variable again
	lineStart.y = position.y - 15;
	lineEnd.y   = position.y + 15;

	lineStart.x = position.x;
	lineEnd.x   = position.x;

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