#include "app.h"
#include <cassert>
#include <functional>

#include "game.h"
#include "main_menu.h"
#include "end_screen.h"


App::App(SceneId sceneId) {
	ChangeTo(sceneId);
}

void App::ChangeTo(SceneId id) {
	assert(id < SceneId::Max);

	using std::placeholders::_1;
	std::function<void(SceneId)> transitionFunc = std::bind(&App::ChangeTo, this, _1);
	
	// TODO Consider making this a non-owning raw ptr, pointing to an already stored scene of each. Re-making each scene on-call
	switch (id) {
	case SceneId::MainMenu:  { currentScene = std::unique_ptr<Scene>(new MainMenu(transitionFunc)); break; }
	case SceneId::Game:		 { currentScene = std::unique_ptr<Scene>(new Game(transitionFunc, leaderboard)); break; }
	case SceneId::EndScreen: { currentScene = std::unique_ptr<Scene>(new EndScreen(transitionFunc, leaderboard)); break; }
	}
}

void App::Update() noexcept {
	currentScene->Update();
}

void App::Render() const noexcept {
	currentScene->Render();
}
