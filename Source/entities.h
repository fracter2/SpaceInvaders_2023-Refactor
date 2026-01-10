#pragma once
#include <concepts>
#include <vector>
#include <algorithm>

#include "common.h"

template<typename T>
concept CanBeActive = requires (T a) {
	{ a.active } -> std::_Boolean_testable;
};


template<typename T> requires CanBeActive<T>
void ClearInactive(std::vector<T>& vec) {
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

//template<typename L, typename C> requires IsCollisionCircle<T>
bool IsColliding(const IsCollisionLine auto& line, const IsCollisionCircle auto& circle) {
	if (!line.active || !circle.active) {
		return false;
	}
	return CheckCollisionCircleLine(circle.position, circle.radius, line.getLineStart(), line.getLineEnd());
}

void CheckAndCollide(IsCollisionLine auto& line, IsCollisionCircle auto& circle) {
	if (IsColliding(line, circle)) {
		line.GetPewd();
		circle.GetPewd();
	}
}