#pragma once
#include "scene.h"
#include <functional>
#include "leaderboard.h"
#include "common.h"
#include <string>

class EndScreen : public Scene {
public:
	EndScreen(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb) noexcept;
	void Update() noexcept override;
	void Render() const noexcept override;

private:
	std::function<void(SceneId)> transitionTo;
	Leaderboard* leaderboard;						// NOTE non-owning ptr		// TODO Consider clarifying not_null
};