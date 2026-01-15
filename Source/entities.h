#pragma once		// TODO Consider making into #ifdef... type include guard (portability, recomended by cpp guidelines)
#include <concepts>
#include <vector>

#include "common.h"
#include "resources.h"


// Collission concepts

// NOTE templated func definitions cannot be delegated to a cpp file apparently (doesn't compile). So the full definition shall stay here.


template<typename T>
concept CanBeActive = requires (T const ct) 
{
	{ ct.IsQueuedForDelete() } noexcept -> std::_Boolean_testable;
};

template<typename T>
concept CanCollide = requires (T c) 
{
	{ c.OnCollision() } noexcept;
};

template<typename T> requires CanBeActive<T>
void ClearInactive(std::vector<T>& vec) noexcept 
{
	vec.erase(
		std::remove_if(vec.begin(), vec.end(),
			[](const T e) { return e.IsQueuedForDelete(); }),
		vec.end()
	);
}

template<typename T> 
concept IsCollisionCircle = requires (T t, T const ct) 
{
	requires CanBeActive<T>;
	requires CanCollide<T>;
	{ ct.GetPosition() } noexcept -> std::convertible_to<Vector2>;
	{ ct.GetRadius() } noexcept -> std::convertible_to<float>;
};

template <typename T>
concept IsCollisionLine = requires (T t) 
{
	requires CanBeActive<T>;
	requires CanCollide<T>;
	{ t.getLineStart() } -> std::convertible_to<Vector2>;
	{ t.getLineEnd() } -> std::convertible_to<Vector2>;
};

bool IsColliding(const IsCollisionLine auto& line, const IsCollisionCircle auto& circle) noexcept 
{
	if (line.IsQueuedForDelete() || circle.IsQueuedForDelete()) {
		return false;
	}
	return CheckCollisionCircleLine(circle.GetPosition(), circle.GetRadius(), line.getLineStart(), line.getLineEnd());
}

void CheckAndCollide(IsCollisionLine auto& line, IsCollisionCircle auto& circle) noexcept 
{
	if (IsColliding(line, circle)) {
		line.OnCollision();
		circle.OnCollision();
	}
}


// Entity types

class Player {
public:
	Player() noexcept;
	void Update() noexcept;
	void Render(const Resources& res) const noexcept;

	// -- IsCollisionCircle concept --
	void OnCollision() noexcept;
	inline Vector2 GetPosition() const noexcept { return position; }			// const func!
	inline constexpr float GetRadius() const noexcept { return 50; }			// constexpr func!
	inline constexpr bool IsQueuedForDelete() const noexcept { return false; }	// NOTE Player can't die
	// --

	inline bool IsDead() const noexcept { return lives <= 0; }
	inline int GetLives() const noexcept { return lives; }

	static constexpr float speed = 7;											// static constexpr must init
	static constexpr float player_y_offset = 70.0f;								// and is easy to reason with

private:
	Vector2 position;
	int lives = 3;
};
static_assert(IsCollisionCircle<Player>);


class Projectile {
public:
	Projectile(Vector2 pos, Vector2 direction, bool fromPlayer) noexcept;
	void Update() noexcept;
	void Render(const Resources& res) const noexcept;

	// -- IsCollisionLine concept -- 
	void OnCollision() noexcept;
	inline Vector2 getLineStart() const noexcept { return Vector2Add(position, lineStartOffset); }	// TODO Add Vector2 '+' overload in common.h
	inline Vector2 getLineEnd() const noexcept	{ return Vector2Add(position, lineEndOffset); }
	inline constexpr bool IsQueuedForDelete() const noexcept { return queueDelete; } 
	// -- 

	inline bool IsFromPlayer() const noexcept { return fromPlayer; }

	static constexpr float speed = 15;
	static constexpr Vector2 lineStartOffset = { 0, 15 };
	static constexpr Vector2 lineEndOffset = { 0, -15 };

private:
	Vector2 position = { 0, 0 };
	Vector2 direction = { 0, 0 };
	bool fromPlayer = false;
	bool queueDelete = false;
};
static_assert(IsCollisionLine<Projectile>);


class Wall {
public:
	explicit Wall(Vector2 pos) noexcept;
	void Render(const Resources& res) const noexcept;

	// -- IsCollisionCircle concept -- 
	void OnCollision() noexcept;
	inline Vector2 GetPosition() const noexcept { return position; }
	inline constexpr float GetRadius() const noexcept { return 60; }
	inline constexpr bool IsQueuedForDelete() const noexcept { return queueDelete; }
	// -- 

private:
	Vector2	position;
	int health = 50;		
	bool queueDelete = false;
};
static_assert(IsCollisionCircle<Wall>);


class Alien {
public:
	explicit Alien(Vector2 pos) noexcept;
	void Update() noexcept;
	void Render(const Resources& res) const noexcept;

	// -- IsCollisionCircle concept --
	void OnCollision() noexcept;
	inline Vector2 GetPosition() const noexcept { return position; }
	inline constexpr float GetRadius() const noexcept { return 30; }
	inline constexpr bool IsQueuedForDelete() const noexcept { return queueDelete; }
	// -- 

	static constexpr int speed = 2;
	static constexpr int heightChangeOnBorderHit = 50;

private:
	Vector2 position;
	bool moveRight = true;				// TODO Rename to clarify it's a variable ("movingRight" or similar) as it sounds like an action (func-like)
	bool queueDelete = false;
};
static_assert(IsCollisionCircle<Alien>);