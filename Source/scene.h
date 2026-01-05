#pragma once

// TODO Consider re-writing these as concepts to provide static-polymorphism
// as it is right now, scenes do not care about SceneManager at all

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

// TODO Reconsider if this is even needed. Scenes themself do not care about this, neither in API nor App itself
struct SceneManager {
	virtual ~SceneManager() = default;
	virtual void QueueTransitionTo(SceneId id) noexcept = 0;	// NOTE Must let Update() and Render() return before transition to avoid use-after-free issues
};