#pragma once
#include "scene.h"
#include <functional>

struct MainMenu : public Scene {		// TODO Make into a class as this is an invariant, and update public/private
	MainMenu(const std::function<void(SceneId)>& transitionFunc) noexcept;

	std::function<void(SceneId)> transitionTo;			// NOTE always return after call	
														// TODO Make it idiot-proof, either force return OR make it safe 
														// by delaying until after Update() is done

	void Update() noexcept override;
	void Render() const noexcept override;
};