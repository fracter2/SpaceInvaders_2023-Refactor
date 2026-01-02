#pragma once

#include <vector>
#include "Resources.h"
#include <string>
#include "scene.h"
#include <functional>

#include "leaderboard.h"


struct Player							// TODO Consider moving to it's own file
{										// TODO Make into class, keep constructor, Render() and Update() public
public:
	float x_pos = 0;
	float speed = 7;					// TODO Make static constexpr
	float player_base_height = 70.0f;	// TODO Make static constexpr // TODO this is used as a y position var. merge with x_pos
	float radius = 50;					// TODO Make static constexpr
	int lives = 3;
	int direction = 0;					// TODO Clarify, consider an enum instead
	int activeTexture = 0;				// TODO Clarify, if this is the texture index used, name it so. Review usage and consider enum
	float timer = 0;					// TODO Clarify

	Player() noexcept;
	void Render(Texture2D texture) const noexcept;
	void Update();
	
};


struct Projectile						// TODO Consider moving to it's own file
{										// TODO Make into class
public: 
	// INITIALIZE PROJECTILE WHILE DEFINING IF ITS PLAYER OR ENEMY 
	Vector2 position = {0,0};			// TODO Add a constructor with position param
	int speed = 15;						// TODO Make static constexpr
	bool active = true;					// TODO Remove, shouldn't be needed if inside vector (consider renaming to queueDelete)
	bool fromPlayer = false;

	// LINE WILL UPDATE WITH POSITION FOR CALCULATIONS
	Vector2 lineStart = { 0, 0 };		// TODO Remove, we already have pos... can make a "length" static constexpr to get start/end points
	Vector2 lineEnd = { 0, 0 };			// TODO Remove

	void Update();

	void Render(Texture2D texture) const noexcept;
};

struct Wall 							// TODO Consider moving to it's own file
{										// TODO Make into class
public: 
	Vector2 position;					// TODO Add constructor with pos param
	bool active; 						// TODO Add default // TODO Is this even needed? why not just remove, if it's in a vector?
	Color color;						// TODO Add default and make static constexpr
	int health = 50;
	int radius = 60;					// TODO make constexpr, doesn't change

	void Render(Texture2D texture) const noexcept;
	void Update(); 
};

struct Alien							// TODO Consider moving to it's own file
{										// TODO Make into class
public:
	
	Color color = WHITE;				// TODO Make static constexpr
	Vector2 position = {0, 0};			// TODO Add constructor with pos param
	int x = 0;							// TODO Remove, positino should already have this
	int y = 0; 							// TODO Remove, positino should already have this
	float radius = 30;					// TODO Make static constexpr
	bool active = true;					// TODO Remove, shouldn't be needed if in a vector
	bool moveRight = true;				// TODO Rename to clarify it's a variable ("movingRight" or similar) as it sounds like an action (func-like)

	int speed = 2; 
		 
	void Update(); 
	void Render(Texture2D texture) const noexcept;
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

	int formationWidth = 8;						// TODO Set static constexpr for all these
	int formationHeight = 5;
	int alienSpacing = 80;
	int formationX = 100;
	int formationY = 50;

	void Update() noexcept override;
	void Render() const noexcept override;

	void SpawnAliens();

	bool CheckCollision(Vector2 circlePos, float circleRadius, Vector2 lineTop, Vector2 lineBottom);	// TODO Rename to clarify that it checks line-circle overlap


	// Entity Storage and Resources


	Player player = {};

	std::vector<Projectile> Projectiles;

	std::vector<Wall> Walls;

	std::vector<Alien> Aliens;
	
	Background background;

};