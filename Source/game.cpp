#include "game.h"
#include <iostream>
#include <vector>
#include "common.h"
#include <algorithm>
#include <concepts>


template<typename T> 
concept CanBeActive = requires (T a) {
	{ a.active } -> std::_Boolean_testable;
};

template<typename T> requires CanBeActive<T>
void ClearInactive(std::vector<T>& vec) {
	vec.erase(
		std::remove_if(vec.begin(), vec.end(),
			[](const T e) { return !e.active; }),
		vec.end()
	);
}


template<typename T>
concept IsCollisionCircle = requires (T a) {
	{ a.position } -> std::convertible_to<Vector2>;
	{ a.radius } -> std::convertible_to<float>;
	{ a.active } -> std::_Boolean_testable;
	{ a.GetPewd() };
};

template<typename T> requires IsCollisionCircle<T>
bool IsColliding(const Projectile& proj, const T& other) {
	if (!proj.active || !other.active) {
		return false; 
	}
	return CheckCollisionCircleLine(other.position, other.radius, proj.getLineStart(), proj.getLineEnd());
}

void CheckAndCollide(Projectile& proj, IsCollisionCircle auto& other) {		// TODO Apply the same formatting to IsColliding template, or vice versa
	if (IsColliding(proj, other)) {
		proj.GetPewd();
		other.GetPewd();
	}
}

Game::Game(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb, Resources& res) noexcept
	: transitionTo(transitionFunc)
	, leaderboard(&lb)
	, resources(&res)
	, background(Background(600))		// TODO Clarify magic number
{

	float wallDistance = (float)GetScreenWidth() / (float)(wallCount + 1);
	for (int i = 0; i < wallCount; i++) {
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
		transitionTo(SceneId::EndScreen);		// TODO Move all End checks together
		return;
	}

	player.Update();							// TODO Move all updates() together
	
	for (Alien& alien : Aliens) {
		alien.Update();

		if (alien.position.y > player.position.y)		// TODO Use algorithm std::any_of
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

	for (Projectile& projectile : Projectiles) {
		projectile.Update();
	}
	for (Wall& wall : Walls) {
		wall.Update();			// TODO Refactor away, only checks HP, move to func or other on-hit logic
	}

	CheckCollisions();
	UpdateScore();

	PlayerPewPew();
	AlienPewPew();

	ClearInactive(Projectiles);
	ClearInactive(Aliens);
	ClearInactive(Walls);
}

void Game::CheckCollisions() {
	for (Projectile& proj : Projectiles) {
		for (Wall& wall : Walls) {
			CheckAndCollide(proj, wall);		// TODO Allow passing a vector to simplify further
		}

		if (proj.fromPlayer) {
			for (Alien& alien : Aliens) {
				CheckAndCollide(proj, alien);
			}
		}
		else {
			CheckAndCollide(proj, player);
		}
	}
}

void Game::UpdateScore() {
	for (Alien& alien : Aliens) {											// TODO Consider algorithm std::for_each 
		if (!alien.active) { leaderboard->currentScore += 100; }		
	}
}

void Game::PlayerPewPew() {
	if (IsKeyPressed(KEY_SPACE))
	{
		static constexpr Vector2 direction = { 0, -1 };
		const Vector2 pos = Vector2Add(player.position, { 0, -60 });
		Projectiles.push_back(Projectile(pos, direction, true));
	}
}

void Game::AlienPewPew() {
	shootTimer += 1;												// TODO Refactor away using GetTime() and modulo
	if (shootTimer > 59) //once per second
	{
		shootTimer = 0;

		static constexpr Vector2 direction = { 0, 1 };
		const Vector2 pos = Vector2Add(Aliens[rand() % Aliens.size()].position, { 0, -60 });
		Projectiles.push_back(Projectile(pos, direction, false));
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

void Player::Update()	// TODO Move all input checks together in here or in a separate func
{						// TODO Rename to "move" or similar
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

void Player::GetPewd() {
	lives -= 1;
}

Projectile::Projectile(Vector2 pos, Vector2 direction, bool fromPlayer) noexcept
	: position(pos)
	, direction(direction)
	, fromPlayer(fromPlayer)
{
}

void Projectile::Update()		// TODO Rename to "move" or similar. Or move all checks here
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

void Projectile::GetPewd() {
	active = false;
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

void Wall::Update()			// TODO Refactor away, attacker already deals with modifying dmg, let it check this too (or make func)
{
	if (health < 1)
	{
		active = false;
	}


}

void Wall::GetPewd() {
	health -= 1;
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

void Alien::GetPewd() {
	active = false;
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