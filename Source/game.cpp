#include "game.h"
#include "common.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <ranges>
#include <concepts>
#include <format>


// Internal helper funcs								// TODO Move concepts to separate file
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

void RenderFullTextureWrap(const Texture2D& texture, Vector2 position, Vector2 size) {
	const Rectangle sourceRect = { 0, 0, (float)texture.width, (float)texture.height };					// TODO Consider making a get func in res
	const Rectangle destinationRect = { position.x, position.y, size.x, size.y, };
	const Vector2 originOffset = { size.x / 2, size.y / 2 };
	DrawTexturePro(texture, sourceRect, destinationRect, originOffset, 0, WHITE);
}

// GAME

Game::Game(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb, const Resources& res) noexcept
	: transitionTo(transitionFunc)
	, leaderboard(&lb)
	, resources(&res)
{
	SpawnWalls();
	SpawnAliens();
	leaderboard->currentScore = 0;
}

void Game::Update() noexcept {
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
	if (IsKeyPressed(KEY_SPACE)) {
		static constexpr Vector2 direction = { 0, -1 };
		const Vector2 pos = Vector2Add(player.position, { 0, -60 });
		Projectiles.push_back(Projectile(pos, direction, true));
	}
}

void Game::AlienPewPew() {
	shootTimer += 1;												// TODO Refactor away using GetTime() and modulo
	if (shootTimer > 59) { //once per second
		shootTimer = 0;

		static constexpr Vector2 spawnOffset = { 0, 60 };
		const Alien& randomAlien = Aliens[rand() % Aliens.size()];				// TODO Check if there's a better way to do this with algorithm or else
		const Vector2 pos = Vector2Add(randomAlien.position, spawnOffset);

		static constexpr Vector2 direction = { 0, 1 };
		Projectiles.push_back(Projectile(pos, direction, false));
	}
}

void Game::Render() const noexcept {
	background.Render(player.position.x);
	
	static constexpr int fontSize = 40;
	static constexpr Vector2 scorePos = { 50, 20 };
	static constexpr Vector2 hpPos = { 50, 70 };
	DrawText(std::format("Score: {}", leaderboard->currentScore).c_str(), scorePos.x, scorePos.y, fontSize, YELLOW);
	DrawText(std::format("Lives: {}", player.lives).c_str(),			  hpPos.x,    hpPos.y,    fontSize, YELLOW);

	const Resources& res = *resources;
	player.Render(res);

	for (const Projectile& p : Projectiles) { p.Render(res); }
	for (const Wall& w : Walls)				{ w.Render(res); }
	for (const Alien& a : Aliens)			{ a.Render(res); }
}

void Game::SpawnAliens() {								// TODO Consider making this a free func since it's only used once
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

// PLAYER

Player::Player() noexcept {
	position = Vector2(
		(float)GetScreenWidth() / 2,
		(float)(GetScreenHeight() - player_y_offset)
	);
	std::cout<< "Find Player -X:" << position.x << "Find Player -Y" << position.y << std::endl;	// TODO Remove, redudant printing

}

// TODO Move all input checks together in here or in a separate func
// TODO Rename to "move" or similar
void Player::Update() {
	int direction = IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT);
	position.x += speed * direction;

	position.x = Clamp(position.x, 0 + radius, GetScreenWidth() - radius);
}

void Player::Render(const Resources& res) const noexcept {
	static constexpr float timePerFrame = 0.4;
	const int i = (int)(GetTime() / timePerFrame) % res.shipTextures.size();
	const auto& frame = res.shipTextures[i];

	static constexpr Vector2 targetSize = { 100, 100 };

	RenderFullTextureWrap(frame, position, targetSize);
}

void Player::GetPewd() {
	lives -= 1;
}

// PROJECTILE

Projectile::Projectile(Vector2 pos, Vector2 direction, bool fromPlayer) noexcept
	: position(pos)
	, direction(direction)
	, fromPlayer(fromPlayer)
{
}

void Projectile::Update() { // TODO Rename to "move" or similar. Or move all checks here
	position = Vector2Add(position, direction * speed);

	if (position.y < 0 || position.y > 1500)		// TODO Clarify magic numbers
	{
		active = false;
	}
}

void Projectile::Render(const Resources& res) const noexcept {
	static constexpr Vector2 targetSize = { 50, 50 };
	RenderFullTextureWrap(res.laserTexture, position, targetSize);
}

void Projectile::GetPewd() {
	active = false;
}

// WALL

Wall::Wall(Vector2 pos) noexcept 
	:position(pos)
{
}

void Wall::Render(const Resources& res) const noexcept { // TODO Make Texture2D&
	static constexpr Vector2 targetSize = { 200, 200 };
	RenderFullTextureWrap(res.barrierTexture, position, targetSize);

	// HP label
	static constexpr Vector2 offset = { -21, 10 };
	static constexpr int fontsSize = 40;
	DrawText(std::format("{}", health).c_str(), position.x + offset.x, position.y + offset.y, fontsSize, RED);
	// TODO Consider using DrawTextPro() to mirror above texture drawing. CON: prob not worth the text/effort when it already is aligned
}

void Wall::GetPewd() {
	health -= 1;
	if (health < 1) {
		active = false;
	}
}

// ALIEN

Alien::Alien(Vector2 pos) noexcept
	: position(pos)
{
}

void Alien::Update() {
	position.x += moveRight ? speed : -speed;

	if (position.x <= 0 || position.x >= GetScreenWidth()) {
		position.x = Clamp(position.x, 0, GetScreenWidth());
		position.y += heightChangeOnBorderHit;
		moveRight = !moveRight;
	}
}

void Alien::Render(const Resources& res) const noexcept {		
	static constexpr Vector2 targetSize = { 100, 100 };
	RenderFullTextureWrap(res.alienTexture, position, targetSize);
}

void Alien::GetPewd() {
	active = false;
}

// BACKGROUND

Star::Star(Vector2 pos, float size) noexcept
	: position(pos)
	, size(size)
{
}

void Star::Render(float offset) const noexcept {
	DrawCircle((int)position.x + offset, (int)position.y, size, color);	// TODO Make sure that this is noexcept

	// TODO Test / assert if size is negative (does it crash?)
	// TODO If it crashes, make sure it's still noexcept by verifying size, making this an invariant (class)
}

Background::Background(int starAmount) {
	for (int i = 0; i < starAmount; i++)
	{
		Vector2 pos = { 
			GetRandomValue(-150, GetScreenWidth() + 150),		// TODO Clarify magic numbers
			GetRandomValue(0, GetScreenHeight())
		};

		float size = GetRandomValue(1, 4) / 2;					// TODO Clarify magic numbers

		Stars.push_back(Star(pos, size));
	}
}

void Background::Render(const float player_x) const noexcept {
	const float offset = abs(player_x) / paralaxRatio;
	for (const Star& star : Stars) {
		star.Render(offset);
	}
}