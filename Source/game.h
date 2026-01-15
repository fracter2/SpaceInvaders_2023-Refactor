#pragma once		// TODO Consider making into #ifdef... type include guard (portability, recomended by cpp guidelines)

#include <vector>
#include <functional>

#include "gsl/pointers"
#include "scene.h"
#include "leaderboard.h"
#include "entities.h"



struct Star {												// TODO Move to separate file
public:
	Star() = delete;
	Star(Vector2 pos, float size) noexcept;
	void Render(float offset) const noexcept;

	Vector2 position = { 0, 0 };
	float size = 0;
};


struct Background {											// TODO Move to separate file
public:
	Background() = delete;
	explicit Background(int starAmount);								// TODO Make noescept
	void Render(const float player_x) const noexcept;

	static constexpr float paralaxRatio = 15;
	static_assert(paralaxRatio != 0, "Avoid dividing by zero. Please");

	std::vector<Star> Stars;					// TODO Consider making fixed-size, maybe with star-count as template
};


// TODO apply const, constexpr and noexcept where applicable
// TODO Consider separating private details into a GameImpl class, or just declaring the funcs in the cpp
class Game: public Scene {
public:
	Game() = delete;
	Game(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb, const Resources& res) noexcept;
	void Update() noexcept override;
	void Render() const noexcept override;

private:
	// TODO Make more of these noexcept by having the vectors be std::inline_vector or similar
	void CheckAlienSpawnConditions() noexcept;
	void CheckEndConditions() noexcept;
	void SpawnAliens();
	void SpawnWalls();
	void PlayerPewPew();
	void AlienPewPew();
	void ApplyCollisions();
	void UpdateScore();

	std::function<void(SceneId)> transitionTo;
	gsl::not_null<Leaderboard*> leaderboard;					// NOTE non-owning
	gsl::not_null<const Resources*> resources;					// NOTE non-owning


	float shootTimer = 0;
	Player player = {};

	static constexpr int BackgroundStarCount = 600;
	Background background = Background(BackgroundStarCount);

	// TODO Make them into fixes-size std::inline_vector's with fixed size (make sure funcs that push()/insert() check size, or use try_push()
	std::vector<Projectile> projectiles;
	std::vector<Wall> walls;
	std::vector<Alien> aliens;
};