#pragma once

#include <memory>

#include "scene.h"
#include "leaderboard.h"

class App: public SceneManager {
public:
	App(SceneId sceneId = SceneId::MainMenu);

	void Update() noexcept;
	void Render() const noexcept;

	void ChangeTo(SceneId id) override;

private:
	std::unique_ptr<Scene> currentScene;
	Leaderboard leaderboard = {};
};
