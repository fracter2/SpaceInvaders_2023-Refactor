#include "game.h"
#include "common.h"

#include <vector>
#include <algorithm>
#include <ranges>
#include <format>
#include "gsl/util"



// GAME

Game::Game(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb, const Resources& res) noexcept
	: transitionTo(transitionFunc)							// Init variables from args...
	, leaderboard(&lb)										// transitionFunc is the callable to switch active scene...
	, resources(&res)										// Since lb and res are persistent, Game only keeps them as gsl::not_null(ptr)
{
	SpawnWalls();											// Delegate related spawn-logic in dedicated funcs (that can be re-used)
	SpawnAliens();											// 
	leaderboard->ResetScore();								// Given a name to "score = 0" since multiple scenes may share leaderboard
}

void Game::Update() noexcept 
{
	CheckAlienSpawnConditions();

	player.Update();											// std::for_each algorithm to the rescue!
	std::ranges::for_each(aliens,	   [](Alien& a)		 { a.Update(); });			// Yes I know this is needlessly verbose and that it can (and should)
	std::ranges::for_each(projectiles, [](Projectile& p) { p.Update(); });			// be a simple foreach loop. I just want to show how cool I am.

	PlayerPewPew();
	AlienPewPew();

	ApplyCollisions();											// Internal range-for loops 
	UpdateScore();

	ClearInactive(projectiles);									// Light-wrapper on std::remove_if and vec.erase()
	ClearInactive(aliens);										//
	ClearInactive(walls);										//

	CheckEndConditions();
}

void Game::CheckAlienSpawnConditions() noexcept 
{
	if (aliens.empty()) { 
		SpawnAliens(); 
	}
}

void Game::CheckEndConditions() noexcept 
{

	auto isAlienBelowPlayer = [this](const Alien& alien) { return alien.GetPosition().y > player.GetPosition().y; };

	if (IsKeyReleased(KEY_Q) || player.IsDead() || std::any_of(aliens.begin(), aliens.end(), isAlienBelowPlayer)) {
		transitionTo(SceneId::EndScreen);
	}
}

void Game::ApplyCollisions() 
{
	for (Projectile& proj : projectiles) {
		for (Wall& wall : walls) {
			CheckAndCollide(proj, wall);					// TODO Allow passing a vector to simplify further
		}

		if (proj.IsFromPlayer()) {
			for (Alien& alien : aliens) {
				CheckAndCollide(proj, alien);
			}
		}
		else {
			CheckAndCollide(proj, player);
		}
	}
}

void Game::UpdateScore() 
{
	for (Alien& alien : aliens) {
		if (alien.IsQueuedForDelete()) { 
			leaderboard->AddScore(100); 
		}		
	}
}

void Game::PlayerPewPew() 
{
	if (IsKeyPressed(KEY_SPACE)) {
		static constexpr Vector2 direction = { 0, -1 };
		static constexpr Vector2 spawnOffset = { 0.f, -60.f };
		const Vector2 pos = Vector2Add(player.GetPosition(), spawnOffset);
		projectiles.push_back(Projectile(pos, direction, true));
	}
}

void Game::AlienPewPew() 
{
	shootTimer += 1;												// TODO Refactor away using GetTime() and modulo
	if (shootTimer > 59) { //once per second
		shootTimer = 0;

		// NOTE Is this ok? Bounds-checking is redundant here since we're using % size
		GSL_SUPPRESS(bounds.4) const Alien& randomAlien = aliens[rand() % aliens.size()];

		static constexpr Vector2 spawnOffset = { 0, 60 };
		const Vector2 pos = Vector2Add(randomAlien.GetPosition(), spawnOffset);

		static constexpr Vector2 direction = { 0, 1 };
		projectiles.push_back(Projectile(pos, direction, false));
	}
}

void Game::Render() const noexcept 
{
	background.Render(player.GetPosition().x);
	
	static constexpr int fontSize = 40;
	static constexpr Vector2 scorePos = { 50.f, 20.f };
	static constexpr Vector2 hpPos = { 50.f, 70.f };
	DrawText(std::format("Score: {}", leaderboard->GetScore()).c_str(), scorePos.x, scorePos.y, fontSize, YELLOW);	// TODO Make cast-wrapper in common.h
	DrawText(std::format("Lives: {}", player.GetLives()).c_str(),		hpPos.x,    hpPos.y,    fontSize, YELLOW);	// TODO Make cast-wrapper in common.h

	const Resources& res = *resources;
	player.Render(res);

	for (const Projectile& p : projectiles) { p.Render(res); }
	for (const Wall& w : walls)				{ w.Render(res); }
	for (const Alien& a : aliens)			{ a.Render(res); }
}
// TODO Consider making this a free func since it's only used once
void Game::SpawnAliens() 
{								
	static constexpr int formationWidth = 8;
	static constexpr int formationHeight = 5;
	static constexpr float alienSpacing = 80.f;
	static constexpr float formationX = 550.f;
	static constexpr float formationY = 50.f;

	for (int row = 0; row < formationHeight; row++) {
		for (int col = 0; col < formationWidth; col++) {
			Vector2 pos = {
				formationX + (col * alienSpacing),
				formationY + (row * alienSpacing)
			};
			aliens.push_back(Alien(pos));
		}
	}
}

void Game::SpawnWalls() 
{
	static constexpr int wallCount = 5;
	const float wallX = gsl::narrow_cast<float>(GetScreenWidth() / (float)(wallCount + 1));
	
	static constexpr int heightOffset = -250;
	const float wallY = gsl::narrow_cast<float>(GetScreenHeight() + heightOffset);

	for (int i = 0; i < wallCount; i++) {
		Vector2 pos = { wallX * (i + 1), wallY };
		walls.push_back(Wall(pos));
	}
}


// BACKGROUND

Star::Star(Vector2 pos, float size) noexcept
	: position(pos)
	, size(size)
{
}

void Star::Render(float offset) const noexcept {
	DrawCircle(gsl::narrow_cast<int>(position.x) + offset, gsl::narrow_cast<int>(position.y), size, GRAY);	// TODO Make sure that this is noexcept
																		// TODO Make a wrapper in common.h

	// TODO Test / assert if size is negative (does it crash? use abs()?)
	// TODO If it crashes, make sure it's still noexcept by verifying size, making this an invariant (class)
}

Background::Background(int starAmount) 
{
	for (int i = 0; i < starAmount; i++)
	{
		static constexpr int offsetMin = -150;
		static constexpr int offsetMax = 150;
		Vector2 pos = { 
			gsl::narrow_cast<float>(GetRandomValue(offsetMin, GetScreenWidth() + offsetMax)),
			gsl::narrow_cast<float>(GetRandomValue(0, GetScreenHeight()))
		};

		static constexpr int sizeMin = 1;
		static constexpr int sizeMax = 4;
		float size = gsl::narrow_cast<float>(GetRandomValue(sizeMin, sizeMax)) / 2.0f;

		Stars.push_back(Star(pos, size));
	}
}

void Background::Render(const float player_x) const noexcept 
{
	const float offset = abs(player_x) / paralaxRatio;
	for (const Star& star : Stars) {
		star.Render(offset);
	}
}