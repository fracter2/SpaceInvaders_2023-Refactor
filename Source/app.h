#pragma once

#include <memory>

#include "scene.h"
#include "leaderboard.h"

struct App: public SceneManager {		// TODO Make into a class since it an invariant (ir it is *not* one?)
	App(SceneId sceneId = SceneId::MainMenu) noexcept;

	void Update() noexcept;
	void Render() const noexcept;

	void ChangeTo(SceneId id) noexcept override;

private:
	std::unique_ptr<Scene> currentScene;
	SceneId currentSceneId;
	Leaderboard leaderboard = {};

	// TODO make persistent data into a struct, currently just the score
};
