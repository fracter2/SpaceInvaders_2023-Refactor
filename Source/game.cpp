#include "game.h"
#include "common.h"

#include <vector>
#include <algorithm>
#include <ranges>
#include <format>



// GAME

Game::Game(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb, const Resources& res) noexcept
	: transitionTo(transitionFunc)
	, leaderboard(&lb)
	, resources(&res)
{
	SpawnWalls();
	SpawnAliens();
	leaderboard->ResetScore();
}

void Game::Update() noexcept 
{
	CheckAlienSpawnConditions();

	player.Update();
	std::ranges::for_each(aliens,	   [](Alien& a)		 { a.Update(); });			// Yes I know this is needlessly verbose and that it can (and should)
	std::ranges::for_each(projectiles, [](Projectile& p) { p.Update(); });			// be a simple foreach loop. I just want to show how cool I am.

	PlayerPewPew();
	AlienPewPew();

	ApplyCollisions();
	UpdateScore();

	ClearInactive(projectiles);
	ClearInactive(aliens);
	ClearInactive(walls);

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
		const Vector2 pos = Vector2Add(player.GetPosition(), {0, -60});
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

void Game::SpawnAliens() 
{								// TODO Consider making this a free func since it's only used once
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
	const float wallDistance = (float)GetScreenWidth() / (float)(wallCount + 1);
	const float wallHeightOffset = (float)GetScreenHeight() - 250;

	for (int i = 0; i < wallCount; i++) {
		Vector2 pos = { wallDistance * (i + 1), wallHeightOffset };
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
	DrawCircle((int)position.x + offset, (int)position.y, size, GRAY);	// TODO Make sure that this is noexcept
																		// TODO Make a wrapper in common.h

	// TODO Test / assert if size is negative (does it crash? use abs()?)
	// TODO If it crashes, make sure it's still noexcept by verifying size, making this an invariant (class)
}

Background::Background(int starAmount) 
{
	for (int i = 0; i < starAmount; i++)
	{
		Vector2 pos = { 
			(float)GetRandomValue(-150, GetScreenWidth() + 150),		// TODO Clarify magic numbers
			(float)GetRandomValue(0, GetScreenHeight())
		};

		float size = (float)GetRandomValue(1, 4) / 2;					// TODO Clarify magic numbers

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