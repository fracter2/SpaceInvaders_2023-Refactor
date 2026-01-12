#pragma once		// TODO Consider making into #ifdef... type include guard (portability, recomended by cpp guidelines)
#include <functional>

#include "gsl/pointers"
#include "scene.h"
#include "leaderboard.h"

class EndScreen : public Scene {
public:
	EndScreen(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb) noexcept;
	void Update() noexcept override;
	void Render() const noexcept override;

private:
	std::function<void(SceneId)> transitionTo;
	gsl::not_null<Leaderboard*> leaderboard;						// NOTE non-owning ptr
};