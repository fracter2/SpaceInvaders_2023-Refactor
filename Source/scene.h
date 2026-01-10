#pragma once

// TODO Consider re-writing these as concepts to provide static-polymorphism 
struct Scene {
	virtual ~Scene() = default;
	virtual void Update() noexcept = 0;
	virtual void Render() const noexcept = 0;
};

enum struct SceneId : int {
	Min,
	MainMenu,
	Game,
	Highscore,
	EndScreen,
	Max
};