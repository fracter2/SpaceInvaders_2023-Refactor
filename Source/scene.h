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
	Max
};


struct SceneManager {
	virtual ~SceneManager() = default;
	virtual void ChangeTo(SceneId id) = 0;
};
