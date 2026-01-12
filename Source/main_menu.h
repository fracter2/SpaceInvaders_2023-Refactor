#pragma once		// TODO Consider making into #ifdef... type include guard (portability, recomended by cpp guidelines)
#include <functional>

#include "scene.h"

class MainMenu : public Scene {
public:
	MainMenu(const std::function<void(SceneId)>& transitionFunc) noexcept;	// TODO Make excplicit (single-argument constructors should be)
	void Update() noexcept override;
	void Render() const noexcept override;

private:
	std::function<void(SceneId)> transitionTo;
};