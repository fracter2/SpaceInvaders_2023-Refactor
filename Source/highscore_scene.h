#pragma once
#include "scene.h"
#include <functional>
#include "leaderboard.h"
#include "common.h"
#include <string>

class HighscoreScene : public Scene { 
public:
	HighscoreScene(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb) noexcept;
	void Update() noexcept override;
	void Render() const noexcept override;

private:
	void ParseNameInput() noexcept;

	std::function<void(SceneId)> transitionTo;
	Leaderboard* leaderboard;						// NOTE non-owning ptr		// TODO Consider clarifying not_null

	std::string name = "";
	Rectangle textBox = { 600, 500, 225, 50 };
	bool mouseOnText = false;
	int framesCounter = 0;																	// TODO Move into textbox class
};