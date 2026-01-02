#pragma once

#include <memory>

#include "scene.h"
#include "leaderboard.h"

class App: public SceneManager {
public:
	App(SceneId sceneId = SceneId::MainMenu) noexcept;

	void Update() noexcept;
	void Render() const noexcept;

	void ChangeTo(SceneId id) noexcept override;

private:
	std::unique_ptr<Scene> currentScene;
	Leaderboard leaderboard = {};
};
