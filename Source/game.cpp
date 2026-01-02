#include "game.h"
#include <iostream>
#include <vector>
#include "common.h"



Game::Game(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb) noexcept
	: transitionTo(transitionFunc)
	, leaderboard(lb)
{
//void Game::Start() // TODO Rename to clarify this initialized the level and changes state to gameplay

	// creating walls	// TODO Capitalize comments and make them Imperative (eg, "create walls") or remove
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


	//creating player
	Player newPlayer;
	player = newPlayer;
	player.Initialize();		// TODO Refactor away 2 step init. Make this all one line

	//creating aliens
	SpawnAliens();
	

	//creating background
	Background newBackground;			// TODO Refactor away 2step init
	newBackground.Initialize(600);		// TODO Clarify magic number
	background = newBackground;

	//reset score
	leaderboard.currentScore = 0;							// TODO Move to dedicated persistent object

	//LOAD SOME RESOURCES HERE			// TODO Remove redudant comment
	resources = {};
	resources.Load();					// TODO Refactor away 2-step init of loading resources. Move to resource constructor, which should be constructed by the Game constructor


}

void Game::Update() noexcept
{
	//Code							// TODO Remove, redudant
	if (IsKeyReleased(KEY_Q))
	{
		transitionTo(SceneId::EndScreen);
	}

	//Update Player					// TODO Remove, redudant
	player.Update();
		
	//Update Aliens and Check if they are past player
	for (int i = 0; i < Aliens.size(); i++) // TODO make into a for each loop
	{
		Aliens[i].Update(); 

		if (Aliens[i].position.y > GetScreenHeight() - player.player_base_height)	// TODO Can't this be simplified to more/less than player y?
		{
			transitionTo(SceneId::EndScreen);
		}
	}

	//End game if player dies		// TODO Remove, redudant
	if (player.lives < 1)			// TODO Shouldn't this go after projectile update?
	{
		transitionTo(SceneId::EndScreen);
	}

	//Spawn new aliens if aliens run out	// TODO Remove, redudant
	if (Aliens.size() < 1)	// TODO Use .empty() for clarity
	{
		SpawnAliens();
	}


	// Update background with offset
	playerPos = { player.x_pos, (float)player.player_base_height };
	cornerPos = { 0, (float)player.player_base_height };
	offset = Vector2Distance(playerPos, cornerPos) * -1;					// TODO Simplify, both args have the same y, and cornerPos.x is always 0... so it's always just abs(playerPos.x)
	background.Update(offset / 15);									// TODO Clarify 15


	//UPDATE PROJECTILE									// TODO Remove, redudant
	for (int i = 0; i < Projectiles.size(); i++)		// TODO Make foreach loop
	{
		Projectiles[i].Update();
	}
	//UPDATE PROJECTILE									// TODO Remove, redudant
	for (int i = 0; i < Walls.size(); i++)				// TODO Make foreach loop
	{
		Walls[i].Update();
	}

	//CHECK ALL COLLISONS HERE
	for (int i = 0; i < Projectiles.size(); i++)
	{
		if (Projectiles[i].type == EntityType::PLAYER_PROJECTILE)
		{
			for (int a = 0; a < Aliens.size(); a++)
			{
				if (CheckCollision(Aliens[a].position, Aliens[a].radius, Projectiles[i].lineStart, Projectiles[i].lineEnd))
				{
					// Kill!
					std::cout << "Hit! \n";
					// Set them as inactive, will be killed later
					Projectiles[i].active = false;
					Aliens[a].active = false;
					leaderboard.currentScore += 100;
				}
			}
		}

		//ENEMY PROJECTILES HERE
		for (int i = 0; i < Projectiles.size(); i++)
		{
			if (Projectiles[i].type == EntityType::ENEMY_PROJECTILE)
			{
				if (CheckCollision({player.x_pos, GetScreenHeight() - player.player_base_height }, player.radius, Projectiles[i].lineStart, Projectiles[i].lineEnd))
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
				// Set them as inactive, will be killed later
				Projectiles[i].active = false;
				Walls[b].health -= 1;
			}
		}
	}

	//MAKE PROJECTILE
	if (IsKeyPressed(KEY_SPACE))
	{
		float window_height = (float)GetScreenHeight();
		Projectile newProjectile;
		newProjectile.position.x = player.x_pos;
		newProjectile.position.y = window_height - 130;
		newProjectile.type = EntityType::PLAYER_PROJECTILE;
		Projectiles.push_back(newProjectile);
	}

	//Aliens Shooting
	shootTimer += 1;
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
		newProjectile.type = EntityType::ENEMY_PROJECTILE;
		Projectiles.push_back(newProjectile);
		shootTimer = 0;
	}

	// REMOVE INACTIVE/DEAD ENITITIES
	for (int i = 0; i < Projectiles.size(); i++)
	{
		if (Projectiles[i].active == false)
		{
			Projectiles.erase(Projectiles.begin() + i);
			// Prevent the loop from skipping an instance because of index changes, since all insances after
			// the killed objects are moved down in index. This is the same for all loops with similar function
			i--;
		}
	}
	for (int i = 0; i < Aliens.size(); i++)
	{
		if (Aliens[i].active == false)
		{
			Aliens.erase(Aliens.begin() + i);
			i--;
		}
	}
	for (int i = 0; i < Walls.size(); i++)
	{
		if (Walls[i].active == false)
		{
			Walls.erase(Walls.begin() + i);
			i--;
		}
	}
}


void Game::Render() const noexcept
{
	//Code																		// TODO Remove, redudant
	//background render LEAVE THIS AT TOP
	background.Render();

	//DrawText("GAMEPLAY", 50, 30, 40, YELLOW);									// TODO Remove, redudant
	DrawText(TextFormat("Score: %i", leaderboard.currentScore), 50, 20, 40, YELLOW);
	DrawText(TextFormat("Lives: %i", player.lives), 50, 70, 40, YELLOW);

	//player rendering															// TODO Remove, redudant
	player.Render(resources.shipTextures[player.activeTexture]);

	//projectile rendering														// TODO Remove, redudant
	for (int i = 0; i < Projectiles.size(); i++)
	{
		Projectiles[i].Render(resources.laserTexture);
	}

	// wall rendering															// TODO Remove, redudant
	for (int i = 0; i < Walls.size(); i++)
	{
		Walls[i].Render(resources.barrierTexture); 
	}

	//alien rendering															// TODO Remove, redudant
	for (int i = 0; i < Aliens.size(); i++)
	{
		Aliens[i].Render(resources.alienTexture);
	}
}

void Game::SpawnAliens()				// TODO Move to ded
{
	for (int row = 0; row < formationHeight; row++) {		// TODO Consider refactoring to remove raw loop / nesting... otherwise fine
		for (int col = 0; col < formationWidth; col++) {
			Alien newAlien = Alien();	// TODO Refactor this below into a constructor (2 step init)
			newAlien.active = true;
			newAlien.position.x = formationX + 450 + (col * alienSpacing);	// TODO Clarify 450 as constexpr or in a comment... is it just the spawn offset?
			newAlien.position.y = formationY + (row * alienSpacing);
			Aliens.push_back(newAlien);
			std::cout << "Find Alien -X:" << newAlien.position.x << std::endl;	// TODO Remove, redundant 
			std::cout << "Find Alien -Y:" << newAlien.position.y << std::endl;	// TODO Remove, redundant
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

void Player::Initialize() // TODO Refactor into a contructor
{
	
	float window_width = (float)GetScreenWidth(); // TODO Remove, redudant, not used more than once
	x_pos = window_width / 2;
	std::cout<< "Find Player -X:" << GetScreenWidth() / 2 << "Find Player -Y" << GetScreenHeight() - player_base_height << std::endl;	// TODO Remove, redudant printing

}

void Player::Update() 
{

	//Movement
	direction = 0;
	if (IsKeyDown(KEY_LEFT))
	{
		direction--;
	}
	if (IsKeyDown(KEY_RIGHT))
	{
		direction++;
	}

	x_pos += speed * direction;

	if (x_pos < 0 + radius)				// TODO Clarify this is border colission
	{
		x_pos = 0 + radius;
	}
	else if (x_pos > GetScreenWidth() - radius)
	{
		x_pos = GetScreenWidth() - radius;
	}


	//Determine frame for animation
	timer += GetFrameTime();

	if (timer > 0.4 && activeTexture == 2)		// TODO Make these numbers into static constexpr vars
	{
		activeTexture = 0;						// TODO Simplify both ifs into one by using modulo to avoid max (2 here)
		timer = 0;
	}
	else if (timer > 0.4)
	{
		activeTexture++;
		timer = 0;
	}

	
}

void Player::Render(Texture2D texture) const noexcept			// TODO Make Texture2D&
{
	float window_height = GetScreenHeight(); // TODO Remove, only used once and doesn't make this any more readable

	// TODO Consider moving some / most / all these args into static constexpr variables
	DrawTexturePro(texture,						// TODO Make sure this is noexcept
		{
			0,
			0,
			352,
			352,
		},
		{
			x_pos, window_height - player_base_height,
			100,
			100,
		}, { 50, 50 },
		0,
		WHITE);
}



void Projectile::Update()
{
	position.y -= speed;

	// UPDATE LINE POSITION				// TODO Remove this block, this is just the position variable again
	lineStart.y = position.y - 15;
	lineEnd.y   = position.y + 15;

	lineStart.x = position.x;
	lineEnd.x   = position.x;

	if (position.y < 0 || position.y > 1500)		// TODO Clarify magic numbers
	{
		active = false;
	}
}

void Projectile::Render(Texture2D texture) const noexcept			// TODO Make Texture2D&
{
	//DrawCircle((int)position.x, (int)position.y, 10, RED);		// TODO Remove old comment
	// TODO Consider moving some / most / all these args into static constexpr variables
	DrawTexturePro(texture,
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

void Wall::Render(Texture2D texture) const noexcept			// TODO Make Texture2D&
{
	// TODO Consider moving some / most / all these args into static constexpr variables
	DrawTexturePro(texture,
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

	// set walls as inactive when out of health		// TODO Remove, redudant
	if (health < 1)
	{
		active = false;
	}


}

void Alien::Update() 
{
	int window_width = GetScreenWidth();	// TODO Remove, isn't even used

	if (moveRight)			// TODO Move statement to just the position.x update, perhaps as ternary operator
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

void Alien::Render(Texture2D texture) const noexcept
{
	//DrawRectangle((int)position.x - 25, (int)position.y, 30, 30, RED);
	//DrawCircle((int)position.x, (int)position.y, radius, GREEN);
	
	
	// TODO Consider moving some / most / all these args into static constexpr variables
	DrawTexturePro(texture,
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
void Star::Update(float starOffset)
{
	position.x = initPosition.x + starOffset;
	position.y = initPosition.y;

}

void Star::Render() const noexcept
{
	DrawCircle((int)position.x, (int)position.y, size, color);	// TODO Make sure that this is noexcept
}


void Background::Initialize(int starAmount)
{
	for (int i = 0; i < starAmount; i++)
	{
		Star newStar;

		newStar.initPosition.x = GetRandomValue(-150, GetScreenWidth() + 150);
		newStar.initPosition.y = GetRandomValue(0, GetScreenHeight());
		
		//random color?
		newStar.color = SKYBLUE;

		newStar.size = GetRandomValue(1, 4) / 2;

		Stars.push_back(newStar);

	}
}

void Background::Update(float offset)
{
	for (int i = 0; i < Stars.size(); i++)
	{
		Stars[i].Update(offset);		// TODO Refactor offset into Background struct because it is the same for all Stars[] elements, so it's irrelevant to them
	}
	
}

void Background::Render() const noexcept
{
	for (int i = 0; i < Stars.size(); i++)
	{
		Stars[i].Render();
	}
}

















/*LEGACY CODE
	// our objective is to calculate the distance between the closest point of the line to the centre of the circle,
	// and determine if it is shorter than the radius.

	// we can imagine the edges of the line and circle centre to form a triangle. calculating the height of the
	// triangle will give us the distance, if the line serves as the base

	// simplify variables
	Vector2 A = lineStart;
	Vector2 B = lineEnd;
	Vector2 C = circlePos;

	// calculate area using determinant method

	float triangle_area = fabsf(A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y)) / 2;


	// Caculate vectors AB to calculate base length
	Vector2 AB;
	AB.x = B.x - A.x;
	AB.y = B.y - A.y;

	//get the base length
	float trangle_base_length = (float)sqrt(pow(AB.x, 2) + pow(AB.y, 2));

	// we double the area to turn in into a rectangle, and then divide the base length to get the height.
	float triangle_height = (triangle_area * 2 ) / trangle_base_length;

	std::cout << triangle_area << "\n";

	if (triangle_height < circleRadius)
	{
		return true;
	}
	else
	{
		return false;
	}


	*/

