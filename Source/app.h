#pragma once

#include <memory>
#include <string_view>

#include "scene.h"
#include "leaderboard.h"
#include "resources.h"

class RaylibLoader {
public:
	RaylibLoader(int screenWidth, int screenHeight, const char* title);
	~RaylibLoader();
};

class App {
public:
	App(SceneId sceneId = SceneId::MainMenu);		// TODO These should also be noexcept!

	void Update() noexcept;
	void Render() const noexcept;
	void QueueTransitionTo(SceneId id) noexcept;

private:
	void ChangeTo(SceneId id);						// TODO Make this noexcept by catching any errors

	static constexpr int screenWidth = 1920;
	static constexpr int screenHeight = 1080;
	static constexpr std::string_view title = { "SPACE INVADERS" };
	RaylibLoader rlLoader = { screenWidth, screenHeight, title.data() };

	std::unique_ptr<Scene> currentScene;
	Leaderboard leaderboard = {};
	Resources resources = {};

	bool transitionQueued = false;
	SceneId queuedId;

	inline bool SceneIsBusy() const noexcept { return inUpdate || inRender; }
	bool inUpdate = false;
	mutable bool inRender = false;
};

