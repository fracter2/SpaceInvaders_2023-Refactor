#pragma once

struct Scene {
	Scene() = delete;						// TODO Check if this does anything, may be redudant with Update() = 0;
	virtual ~Scene() = default;
	virtual void Update() = 0;
	virtual void Render() = 0;
};

enum struct SceneId : int {
	MainMenu,
	Game,
	EndScreen,
	Max
};

struct SceneManager {
	SceneManager() = delete;				// TODO Check if this does anything, may be redudant with ChangeTo() = 0;
	virtual ~SceneManager() = default;
	virtual void ChangeTo(SceneId id) = 0;
};

