#pragma once
#include <concepts>

#include "common.h"
#include "resources.h"


// Collission concepts

// NOTE templated func definitions cannot be delegated to a cpp file apparently (doesn't compile). So the full definition shall stay here.

template<typename T>
concept CanBeActive = requires (T a) {
	{ a.IsQueuedForDelete() } -> std::_Boolean_testable;
};

template<typename T>
concept CanCollide = requires (T c) {
	{ c.OnCollision() };
};

template<typename T> requires CanBeActive<T>
void ClearInactive(std::vector<T>& vec) {					// TODO Consider noexcept
	vec.erase(
		std::remove_if(vec.begin(), vec.end(),
			[](const T e) { return e.IsQueuedForDelete(); }),
		vec.end()
	);
}

template<typename T> 
concept IsCollisionCircle = requires (T a) {
	requires CanBeActive<T>;
	requires CanCollide<T>;
	{ a.position } -> std::convertible_to<Vector2>;			// TODO Consider making func to allow private
	{ a.radius } -> std::convertible_to<float>;				// TODO Consider making func to alllow private
};

template <typename T>
concept IsCollisionLine = requires (T a) {
	requires CanBeActive<T>;
	requires CanCollide<T>;
	{ a.getLineStart() } -> std::convertible_to<Vector2>;
	{ a.getLineEnd() } -> std::convertible_to<Vector2>;
};

bool IsColliding(const IsCollisionLine auto& line, const IsCollisionCircle auto& circle) {	// TODO Consider noexcept
	if (line.IsQueuedForDelete() || circle.IsQueuedForDelete()) {
		return false;
	}
	return CheckCollisionCircleLine(circle.position, circle.radius, line.getLineStart(), line.getLineEnd());
}

void CheckAndCollide(IsCollisionLine auto& line, IsCollisionCircle auto& circle) { 			// TODO Consider noexcept
	if (IsColliding(line, circle)) {
		line.OnCollision();
		circle.OnCollision();
	}
}


// Entity types

struct Player
{											// TODO Make into class, keep constructor, Render() and Update() public
public:
	Player() noexcept;
	void Update();
	void Render(const Resources& res) const noexcept;

	static constexpr float speed = 7;
	static constexpr float player_y_offset = 70.0f;

	int lives = 3;

	// IsCollisionCircle concept
	void OnCollision();
	static constexpr float radius = 50;
	constexpr bool IsQueuedForDelete() const noexcept { return false; }
	Vector2 position;
};
static_assert(IsCollisionCircle<Player>);


struct Projectile						// TODO Consider moving to it's own file
{
public:
	Projectile(Vector2 pos, Vector2 direction, bool fromPlayer) noexcept;
	void Update();
	void Render(const Resources& res) const noexcept;

	static constexpr float speed = 15;
	static constexpr Vector2 lineStartOffset = { 0, 15 };
	static constexpr Vector2 lineEndOffset = { 0, -15 };

	Vector2 direction = { 0, 0 };
	bool fromPlayer = false;

	// IsCollisionLine concept
	void OnCollision();
	inline Vector2 getLineStart() const { return Vector2Add(position, lineStartOffset); }	// TODO Add Vector2 '+' overload in common.h
	inline Vector2 getLineEnd() const	{ return Vector2Add(position, lineEndOffset); }
	inline constexpr bool IsQueuedForDelete() const noexcept { return queueDelete; } 
	Vector2 position = { 0, 0 };			// TODO Remove default init, to ephasise constructor more

private:
	bool queueDelete = false;
};
static_assert(IsCollisionLine<Projectile>);


struct Wall			// TODO Make class, is invariant as health corelates to active
{
public:
	Wall(Vector2 pos) noexcept;								// TODO Make excplicit (single-argument constructors should be)
	void Render(const Resources& res) const noexcept;

	int health = 50;		

	// IsCollisionCircle concept
	void OnCollision();
	static constexpr int radius = 60;
	inline constexpr bool IsQueuedForDelete() const noexcept { return queueDelete; }
	Vector2	position;

private:
	bool queueDelete = false;
};
static_assert(IsCollisionCircle<Wall>);


struct Alien
{
public:
	Alien(Vector2 pos) noexcept;							// TODO Make excplicit (single-argument constructors should be)
	void Update();
	void Render(const Resources& res) const noexcept;

	static constexpr int speed = 2;
	static constexpr int heightChangeOnBorderHit = 50;

	bool moveRight = true;				// TODO Rename to clarify it's a variable ("movingRight" or similar) as it sounds like an action (func-like)

	// IsCollisionCircle concept
	void OnCollision();
	static constexpr float radius = 30;
	inline constexpr bool IsQueuedForDelete() const noexcept { return queueDelete; }
	Vector2 position = { 0, 0 };

private:
	bool queueDelete = false;
};
static_assert(IsCollisionCircle<Alien>);