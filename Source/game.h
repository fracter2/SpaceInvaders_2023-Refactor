#pragma once

#include <vector>
#include "Resources.h"
#include <string>
#include "scene.h"
#include <functional>

#include "leaderboard.h"
#include "common.h"

struct Player							// TODO Consider moving to it's own file
{										// TODO Make into class, keep constructor, Render() and Update() public
public:
	Vector2 position = {};
	float speed = 7;					// TODO Make static constexpr
	static constexpr float player_y_offset = 70.0f;
	float radius = 50;					// TODO Make static constexpr
	int lives = 3;
	int activeTexture = 0;				// TODO Clarify, if this is the texture index used, name it so. Review usage and consider enum
	float timer = 0;					// TODO Clarify

	Player() noexcept;
	void Render(const Resources& res) const noexcept;
	void Update();
	
};


struct Projectile						// TODO Consider moving to it's own file
{										// TODO Make into class
public: 
	// INITIALIZE PROJECTILE WHILE DEFINING IF ITS PLAYER OR ENEMY 
	Vector2 position = { 0, 0 };			// TODO Add a constructor with position param
	bool active = true;						// TODO Remove, shouldn't be needed if inside vector (consider renaming to queueDelete)
	bool fromPlayer = false;
	Vector2 direction = { 0, 0 };
	static constexpr float speed = 15;
	static constexpr Vector2 lineStartOffset = { 0, 15 };
	static constexpr Vector2 lineEndOffset = { 0, -15 };

	inline Vector2 getLineStart() const { return Vector2Add(position, lineStartOffset); }	// TODO Consider adding Vector2 '+' overload in common.h
	inline Vector2 getLineEnd() const { return Vector2Add(position, lineEndOffset); }
	void Update();
	void Render(const Resources& res) const noexcept;
};

struct Wall 							// TODO Consider moving to it's own file
{										// TODO Make into class
public: 
	Vector2 position;					// TODO Add constructor with pos param
	bool active; 						// TODO Add default // TODO Is this even needed? why not just remove, if it's in a vector?
	Color color;						// TODO Add default and make static constexpr
	int health = 50;
	int radius = 60;					// TODO make constexpr, doesn't change

	Wall(Vector2 pos) noexcept;
	void Render(const Resources& res) const noexcept;
	void Update(); 
};

struct Alien							// TODO Consider moving to it's own file
{										// TODO Make into class
public:
	Vector2 position = {0, 0};
	bool active = true;
	bool moveRight = true;				// TODO Rename to clarify it's a variable ("movingRight" or similar) as it sounds like an action (func-like)

	static constexpr int speed = 2;
	static constexpr Color color = WHITE;
	static constexpr float radius = 30;

	Alien(Vector2 pos) noexcept;
	void Update(); 
	void Render(const Resources& res) const noexcept;
};


struct Star
{
	Star(Vector2 pos, float size) noexcept;
	void Render(float offset) const noexcept;

	static constexpr Color color = GRAY;
	Vector2 position = { 0, 0 };
	float size = 0;
};

struct Background	// TODO Consider moving to separate file
{
	std::vector<Star> Stars;
	float offset = 0;

	Background(int starAmount);
	void Render() const noexcept;
};

// TODO apply const, constexpr and noexcept where applicable
struct Game: public Scene
{
	Game(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb, Resources& res) noexcept;

	std::function<void(SceneId)> transitionTo;
	Leaderboard* leaderboard;					// NOTE non-owning
	const Resources* resources;					// NOTE non-owning, 

	int wallCount = 5;							// TODO Clarify if const or if can be changed

	//Aliens shooting							// TODO Clarify comment, then if logic already is self-explanatory, remove
	float shootTimer = 0;

	void Update() noexcept override;
	void Render() const noexcept override;
	void SpawnAliens();

	// Entity Storage and Resources
	Player player = {};
	std::vector<Projectile> Projectiles;
	std::vector<Wall> Walls;
	std::vector<Alien> Aliens;
	Background background;

};