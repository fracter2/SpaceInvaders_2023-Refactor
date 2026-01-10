#pragma once

#include <vector>
#include <string>
#include <functional>

#include "scene.h"
#include "leaderboard.h"

#include "entities.h"



struct Star
{
public:
	Star(Vector2 pos, float size) noexcept;
	void Render(float offset) const noexcept;

	Vector2 position = { 0, 0 };
	float size = 0;
};


struct Background	// TODO Consider moving to separate file
{
public:
	Background(int starAmount);
	void Render(const float player_x) const noexcept;

	static constexpr float paralaxRatio = 15;
	static_assert(paralaxRatio != 0, "Avoid dividing by zero. Please");

	std::vector<Star> Stars;					// TODO Consider making fixed-size, maybe with star-count as template
};


// TODO apply const, constexpr and noexcept where applicable
// TODO Consider separating private details into a GameImpl class, or just declaring the funcs in the cpp
class Game: public Scene
{
public:
	Game(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb, const Resources& res) noexcept;
	void Update() noexcept override;
	void Render() const noexcept override;

private:
	void CheckAlienSpawnConditions() noexcept;
	void CheckEndConditions() noexcept;
	void SpawnAliens();
	void SpawnWalls();
	void PlayerPewPew();
	void AlienPewPew();
	void ApplyCollisions();
	void UpdateScore();

	std::function<void(SceneId)> transitionTo;
	Leaderboard* leaderboard;					// NOTE non-owning		// TODO Consider std::reference_wrapper<> instead (con: have to .get() )...
	const Resources* resources;					// NOTE non-owning		// ... or use gsl::non_owning, or make an explicicly non_owning ptr templ type

	float shootTimer = 0;
	Player player = {};
	static constexpr int BackgroundStarCount = 600;
	Background background = Background(BackgroundStarCount);

	// TODO Consider making a fixed-size container "object pool" or similar, for these don't actually need resizing
	std::vector<Projectile> Projectiles;		// TODO Correct capitalization
	std::vector<Wall> Walls;					// TODO Correct capitalization
	std::vector<Alien> Aliens;					// TODO Correct capitalization
};