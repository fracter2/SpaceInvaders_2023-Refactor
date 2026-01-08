#pragma once
#include "scene.h"
#include <functional>

class MainMenu : public Scene {
public:
	MainMenu(const std::function<void(SceneId)>& transitionFunc) noexcept;
	void Update() noexcept override;
	void Render() const noexcept override;

private:
	std::function<void(SceneId)> transitionTo;
};