#include "entities.h"
#include <algorithm>

template<typename T> requires CanBeActive<T>
void ClearInactive(std::vector<T>& vec) {
	vec.erase(
		std::remove_if(vec.begin(), vec.end(),
			[](const T e) { return !e.active; }),
		vec.end()
	);
}

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