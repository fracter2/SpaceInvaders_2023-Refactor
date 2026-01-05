#pragma once
#include "scene.h"
#include <functional>
#include "leaderboard.h"
#include "common.h"

struct EndScreen : public Scene {					// TODO Make into a class as this is an invariant
	EndScreen(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb) noexcept;

	// TODO Make these private
	std::function<void(SceneId)> transitionTo;
	Leaderboard& leaderboard;						// TODO Refactor away using raw pointer

	void Update() noexcept override;
	void Render() const noexcept override;

private:
	//bool newHighScore = false;					// TODO Consider renaming to "queueNewHighscore" or "newHighscoreTip"
	

	//TEXTBOX ENTER	
	char name[9 + 1] = "\0";      //One extra space required for null terminator char '\0'	// TODO Make into string
	int letterCount = 0;																	// TODO Remove, redundant if name is a string

	Rectangle textBox = { 600, 500, 225, 50 };
	bool mouseOnText = false;																// TODO Remove, used as a local variable

	int framesCounter = 0;																	// TODO Move into textbox class


};