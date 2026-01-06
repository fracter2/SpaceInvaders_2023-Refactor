#include "game.h"
#include "common.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <ranges>
#include <concepts>



// Internal helper funcs								// TODO Consider moving to separate file
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


// Core implementation
Game::Game(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb, const Resources& res) noexcept
	: transitionTo(transitionFunc)
	, leaderboard(&lb)
	, resources(&res)
{
	SpawnWalls();
	SpawnAliens();
	leaderboard->currentScore = 0;
}

void Game::Update() noexcept
{
	CheckAlienSpawnConditions();

	player.Update();
	std::ranges::for_each(Aliens,	   [](Alien& a)		 { a.Update(); });			// Yes I know this is needlessly verbose and that it can (and should)
	std::ranges::for_each(Projectiles, [](Projectile& p) { p.Update(); });			// be a simple foreach loop. I just want to show how cool I am.

	PlayerPewPew();
	AlienPewPew();

	ApplyCollisions();
	UpdateScore();

	ClearInactive(Projectiles);
	ClearInactive(Aliens);
	ClearInactive(Walls);

	CheckEndConditions();
}

void Game::CheckAlienSpawnConditions() noexcept {
	if (Aliens.empty()) { 
		SpawnAliens(); 
	}
}

void Game::CheckEndConditions() noexcept {

	auto isAlienBelowPlayer = [this](const Alien& alien) { return alien.position.y > player.position.y; };

	if (IsKeyReleased(KEY_Q) || player.lives < 1 || std::any_of(Aliens.begin(), Aliens.end(), isAlienBelowPlayer)) {
		transitionTo(SceneId::EndScreen);
	}
}

void Game::ApplyCollisions() {
	for (Projectile& proj : Projectiles) {
		for (Wall& wall : Walls) {
			CheckAndCollide(proj, wall);					// TODO Allow passing a vector to simplify further
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
	for (Alien& alien : Aliens) {
		if (!alien.active) { 
			leaderboard->currentScore += 100; 
		}		
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
	background.Render(player.position.x);

	DrawText(TextFormat("Score: %i", leaderboard->currentScore), 50, 20, 40, YELLOW);
	DrawText(TextFormat("Lives: %i", player.lives), 50, 70, 40, YELLOW);

	const Resources& res = *resources;
	player.Render(res);

	for (const Projectile& p : Projectiles) { p.Render(res); }
	for (const Wall& w : Walls)				{ w.Render(res); }
	for (const Alien& a : Aliens)			{ a.Render(res); }
}

void Game::SpawnAliens()		// TODO Consider making this a free func since it's only used once
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

void Game::SpawnWalls() {
	static constexpr int wallCount = 5;
	const float wallDistance = (float)GetScreenWidth() / (float)(wallCount + 1);
	const float wallHeightOffset = (float)GetScreenHeight() - 250;

	for (int i = 0; i < wallCount; i++) {
		Vector2 pos = { wallDistance * (i + 1), wallHeightOffset };
		Walls.push_back(Wall(pos));
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
	const int i = (int)(GetTime() / timePerFrame) % res.shipTextures.size();
	const auto& frame = res.shipTextures[i];

	static constexpr Vector2 targetSize = { 100, 100 };

	const Rectangle sourceRect = { 0, 0, (float)frame.width, (float)frame.height };					// TODO Consider making a get func in res
	const Rectangle destinationRect = { position.x, position.y, targetSize.x, targetSize.y, };
	const Vector2 originOffset = { targetSize.x / 2, targetSize.y / 2 };

	DrawTexturePro(frame, sourceRect, destinationRect, originOffset, 0, WHITE);						// TODO Make sure this is noexcept
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
	const auto& texture = res.laserTexture;
	const Rectangle sourceRect = { 0, 0, (float)texture.width, (float)texture.height };					// TODO Consider making a get func in res
	
	static constexpr Vector2 targetSize = { 50, 50 };
	const Rectangle destinationRect = { position.x, position.y, targetSize.x, targetSize.y, };
	const Vector2 originOffset = { targetSize.x / 2, targetSize.y / 2 };

	DrawTexturePro(texture, sourceRect, destinationRect, originOffset, 0, WHITE);
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
	const auto& texture = res.barrierTexture;
	const Rectangle sourceRect = { 0, 0, (float)texture.width, (float)texture.height };					// TODO Consider making a get func in res

	static constexpr Vector2 targetSize = { 200, 200 };
	const Rectangle destinationRect = { position.x, position.y, targetSize.x, targetSize.y, };
	const Vector2 originOffset = { targetSize.x / 2, targetSize.y / 2 };

	DrawTexturePro(texture, sourceRect, destinationRect, originOffset, 0, WHITE);

	// HP label
	static constexpr Vector2 offset = { -21, 10 };
	static constexpr int fontsSize = 40;
	DrawText(TextFormat("%i", health), position.x + offset.x, position.y + offset.y, fontsSize, RED);
	// TODO Consider using modern std::format("{}", health);
	// TODO Consider using DrawTextPro() to mirror above texture drawing
}

void Wall::GetPewd() {
	health -= 1;
	if (health < 1) {
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
			GetRandomValue(-150, GetScreenWidth() + 150),		// TODO Clarify magic numbers
			GetRandomValue(0, GetScreenHeight())
		};

		float size = GetRandomValue(1, 4) / 2;

		Stars.push_back(Star(pos, size));
	}
}

void Background::Render(const float player_x) const noexcept {
	const float offset = abs(player_x) / paralaxRatio;
	for (const Star& star : Stars) {
		star.Render(offset);
	}
}