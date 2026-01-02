#pragma once
#include "scene.h"
#include <functional>

struct MainMenu : public Scene {
	MainMenu(const std::function<void(SceneId)>& transitionFunc) noexcept;

	std::function<void(SceneId)> transitionTo;

	void Update() noexcept override;
	void Render() const noexcept override;
};