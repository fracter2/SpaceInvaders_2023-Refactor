#pragma once
#include "scene.h"
#include <functional>
#include "gsl/pointers"
#include "leaderboard.h"
#include "common.h"
#include <string>

class HighscoreScene : public Scene { 
public:
	HighscoreScene(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb) noexcept;
	void Update() noexcept override;
	void Render() const noexcept override;

	static constexpr int maxNameLength = 9;

private:
	std::function<void(SceneId)> transitionTo;
	gsl::not_null<Leaderboard*> leaderboard;						// NOTE non-owning ptr
	
	std::string name = "";
	static constexpr Rectangle textBox = { 600, 500, 225, 50 };
	bool mouseOnText = false;
	int framesCounter = 0;																	// TODO Move into textbox class
};