#pragma once


struct Scene {
	virtual ~Scene() = default;
	virtual void Update() noexcept = 0;
	virtual void Render() const noexcept = 0;
};


enum struct SceneId : int {
	MainMenu,
	Game,
	EndScreen,
	Max							// TODO Is this even needed? Is it already enforeced to be in-range by being an enum?
};


struct SceneManager {
	virtual ~SceneManager() = default;
	virtual void QueueTransitionTo(SceneId id) noexcept = 0;	// NOTE Must let Update() and Render() return before transition to avoid use-after-free issues
};