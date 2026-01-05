#include "app.h"
#include <cassert>
#include <functional>

#include "game.h"
#include "main_menu.h"
#include "end_screen.h"


App::App(SceneId sceneId) 
	: queuedId(sceneId)
{
	ChangeTo(sceneId);
}

void App::QueueTransitionTo(SceneId id) {
	queuedId = id;
	transitionQueued = true;
}

void App::ChangeTo(SceneId id) {
	assert(!SceneIsBusy());
	assert(id < SceneId::Max);

	using std::placeholders::_1;
	std::function<void(SceneId)> transitionFunc = std::bind(&App::QueueTransitionTo, this, _1);
	
	// TODO Consider making this a non-owning raw ptr, pointing to an already stored scene of each. Re-making each scene on-call
	switch (id) {
	case SceneId::MainMenu:  { currentScene = std::unique_ptr<Scene>(new MainMenu(transitionFunc)); break; }
	case SceneId::Game:		 { currentScene = std::unique_ptr<Scene>(new Game(transitionFunc, leaderboard, resources)); break; }
	case SceneId::EndScreen: { currentScene = std::unique_ptr<Scene>(new EndScreen(transitionFunc, leaderboard)); break; }
	}
}

void App::Update() noexcept {	// TODO Reconsider if this can really be noexcept
	inUpdate = true;
	currentScene->Update();			// TODO Consider try-catching any errors here and printing as an error, or otherwise crashing
									// Deal with it by... printing, freezing screen? going to a "something went wrong" screen?
	inUpdate = false;

	if (transitionQueued) {			// TODO Catch exceptions
		transitionQueued = false;
		ChangeTo(queuedId);
	}
}

void App::Render() const noexcept {
	inRender = true;
	currentScene->Render();
	inRender = false;
}
