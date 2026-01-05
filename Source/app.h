#pragma once

#include <memory>

#include "scene.h"
#include "leaderboard.h"
#include "resources.h"

class App: public SceneManager {
public:
	App(SceneId sceneId = SceneId::MainMenu);

	void Update() noexcept;
	void Render() const noexcept;
	void QueueTransitionTo(SceneId id) noexcept override;	

private:
	void ChangeTo(SceneId id);						// TODO Make this noexcept by catching any errors

	std::unique_ptr<Scene> currentScene;
	Leaderboard leaderboard = {};
	Resources resources = {};

	bool transitionQueued = false;
	SceneId queuedId;

	inline bool SceneIsBusy() const noexcept { return inUpdate || inRender; }
	bool inUpdate = false;
	mutable bool inRender = false;
};
