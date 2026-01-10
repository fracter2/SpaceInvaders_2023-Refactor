#pragma once
#include <concepts>

#include "common.h"
#include "resources.h"


// Collission concepts

// NOTE templated func definitions cannot be delegated to a cpp file apparently (doesn't compile). So the full definition shall stay here.

template<typename T>
concept CanBeActive = requires (T a) {
	{ a.active } -> std::_Boolean_testable;
};

template<typename T> requires CanBeActive<T>
void ClearInactive(std::vector<T>& vec) {					// TODO Consider noexcept
	vec.erase(
		std::remove_if(vec.begin(), vec.end(),
			[](const T e) { return !e.active; }),
		vec.end()
	);
}

template<typename T> 
concept IsCollisionCircle = requires (T a) {
	CanBeActive<T>;
	{ a.position } -> std::convertible_to<Vector2>;
	{ a.radius } -> std::convertible_to<float>;
	{ a.GetPewd() };										// TODO Add to CanBeActive concept and rename to "Collidable"
};

template <typename T>
concept IsCollisionLine = requires (T a) {
	CanBeActive<T>;
	{ a.getLineStart() } -> std::convertible_to<Vector2>;
	{ a.getLineEnd() } -> std::convertible_to<Vector2>;
	{ a.GetPewd() };
};

bool IsColliding(const IsCollisionLine auto& line, const IsCollisionCircle auto& circle) {	// TODO Consider noexcept
	if (!line.active || !circle.active) {
		return false;
	}
	return CheckCollisionCircleLine(circle.position, circle.radius, line.getLineStart(), line.getLineEnd());
}

void CheckAndCollide(IsCollisionLine auto& line, IsCollisionCircle auto& circle) { 			// TODO Consider noexcept
	if (IsColliding(line, circle)) {
		line.GetPewd();
		circle.GetPewd();
	}
}


// Entity types

struct Player								// TODO Consider moving to it's own file
{											// TODO Make into class, keep constructor, Render() and Update() public
public:
	Player() noexcept;
	void Update();
	void Render(const Resources& res) const noexcept;

	static constexpr float speed = 7;
	static constexpr float player_y_offset = 70.0f;

	int lives = 3;
	int activeTexture = 0;					// TODO Clarify, if this is the texture index used, name it so. Review usage and consider enum
	float timer = 0;						// TODO Clarify

	// IsCollisionCircle concept
	void GetPewd();
	static constexpr float radius = 50;
	static constexpr bool active = true;	// NOTE only here to satisfy concept IsCollisionCircle
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
	void GetPewd();
	inline Vector2 getLineStart() const { return Vector2Add(position, lineStartOffset); }	// TODO Consider adding Vector2 '+' overload in common.h
	inline Vector2 getLineEnd() const { return Vector2Add(position, lineEndOffset); }
	bool active = true;						// TODO Cconsider renaming to queueDelete
	Vector2 position = { 0, 0 };			// TODO Remove default init, to ephasise constructor more
};
static_assert(IsCollisionLine<Projectile>);


struct Wall
{
public:
	Wall(Vector2 pos) noexcept;
	void Render(const Resources& res) const noexcept;

	int health = 50;

	// IsCollisionCircle concept
	void GetPewd();
	static constexpr int radius = 60;
	bool	active = true;
	Vector2	position;
};
static_assert(IsCollisionCircle<Wall>);


struct Alien
{
public:
	Alien(Vector2 pos) noexcept;
	void Update();
	void Render(const Resources& res) const noexcept;

	static constexpr int speed = 2;
	static constexpr int heightChangeOnBorderHit = 50;

	bool moveRight = true;				// TODO Rename to clarify it's a variable ("movingRight" or similar) as it sounds like an action (func-like)

	// IsCollisionCircle concept
	void GetPewd();
	static constexpr float radius = 30;
	bool active = true;
	Vector2 position = { 0, 0 };
};
static_assert(IsCollisionCircle<Alien>);