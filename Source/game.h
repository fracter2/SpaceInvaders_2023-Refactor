#pragma once

#include <vector>
#include "Resources.h"
#include <string>
#include "scene.h"
#include <functional>

#include "leaderboard.h"
#include "common.h"
#include "entity_concepts.h"

struct Player								// TODO Consider moving to it's own file
{											// TODO Make into class, keep constructor, Render() and Update() public
public:
	Player() noexcept;
	void Update();
	void Render(const Resources& res) const noexcept;
	void GetPewd();

	static constexpr bool active = true;	// NOTE only here to satisfy concept IsCollisionCircle
	static constexpr float speed = 7;
	static constexpr float player_y_offset = 70.0f;
	static constexpr float radius = 50;

	Vector2 position;
	int lives = 3;
	int activeTexture = 0;					// TODO Clarify, if this is the texture index used, name it so. Review usage and consider enum
	float timer = 0;						// TODO Clarify
};


struct Projectile						// TODO Consider moving to it's own file
{
public: 
	Projectile(Vector2 pos, Vector2 direction, bool fromPlayer) noexcept;
	void Update();
	void Render(const Resources& res) const noexcept;

	void GetPewd();
	inline Vector2 getLineStart() const { return Vector2Add(position, lineStartOffset); }	// TODO Consider adding Vector2 '+' overload in common.h
	inline Vector2 getLineEnd() const { return Vector2Add(position, lineEndOffset); }

	static constexpr float speed = 15;
	static constexpr Vector2 lineStartOffset = { 0, 15 };
	static constexpr Vector2 lineEndOffset = { 0, -15 };

	// INITIALIZE PROJECTILE WHILE DEFINING IF ITS PLAYER OR ENEMY 
	Vector2 position = { 0, 0 };			// TODO Remove default init, to ephasise constructor more
	Vector2 direction = { 0, 0 };
	bool active = true;						// TODO Cconsider renaming to queueDelete
	bool fromPlayer = false;

};

struct Wall
{
public: 
	Wall(Vector2 pos) noexcept;
	void Render(const Resources& res) const noexcept;
	void GetPewd();

	static constexpr int radius = 60;
	
	bool active = true;
	Vector2 position;
	int health = 50;
};

struct Alien
{
public:
	Alien(Vector2 pos) noexcept;
	void Update(); 
	void Render(const Resources& res) const noexcept;
	void GetPewd();

	static constexpr int speed = 2;
	static constexpr int heightChangeOnBorderHit = 50; 
	static constexpr float radius = 30;

	Vector2 position = {0, 0};
	bool active = true;
	bool moveRight = true;				// TODO Rename to clarify it's a variable ("movingRight" or similar) as it sounds like an action (func-like)
};


struct Star
{
	Star(Vector2 pos, float size) noexcept;
	void Render(float offset) const noexcept;

	Vector2 position = { 0, 0 };
	float size = 0;
};

struct Background	// TODO Consider moving to separate file
{
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