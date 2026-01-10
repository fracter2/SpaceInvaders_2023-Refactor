#pragma once
#include <concepts>
#include <vector>

#include "common.h"


template<typename T>
concept CanBeActive = requires (T a) {
	{ a.active } -> std::_Boolean_testable;
};

template<typename T> requires CanBeActive<T>
void ClearInactive(std::vector<T>& vec);					// TODO Consider noexcept

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

bool IsColliding(const IsCollisionLine auto& line, const IsCollisionCircle auto& circle);	// TODO Consider noexcept

void CheckAndCollide(IsCollisionLine auto& line, IsCollisionCircle auto& circle); 			// TODO Consider noexcept