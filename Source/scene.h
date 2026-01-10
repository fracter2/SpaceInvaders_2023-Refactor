#pragma once

// TODO Consider re-writing these as concepts to provide static-polymorphism 
struct Scene {
	virtual ~Scene() = default;
	virtual void Update() noexcept = 0;
	virtual void Render() const noexcept = 0;
};

enum struct SceneId : int {
	MainMenu,
	Game,
	Highscore,
	EndScreen,
	Max							// TODO Is this even needed? Is it already enforeced to be in-range by being an enum?
};