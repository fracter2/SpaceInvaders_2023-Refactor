#include "app.h"
#include <cassert>
#include <functional>

#include "game.h"
#include "main_menu.h"
#include "end_screen.h"
#include "highscore_scene.h"


App::App(SceneId sceneId) 
	: queuedId(sceneId)
	, leaderboard(GetExampleLeaderboard())
{
	SetTargetFPS(60);

	ChangeTo(sceneId);
}

void App::QueueTransitionTo(SceneId id) noexcept {
	queuedId = id;
	transitionQueued = true;
}

void App::ChangeTo(SceneId id) {
	assert(!SceneIsBusy());
	assert(SceneId::Min < id && id < SceneId::Max);

	std::function<void(SceneId)> transitionFunc = std::bind_front(&App::QueueTransitionTo, this);		
	
	// TODO Consider making this a non-owning raw ptr, pointing to an already stored scene of each. Re-making each scene on-call
	switch (id) {
	case SceneId::MainMenu:  { currentScene = std::unique_ptr<Scene>(new MainMenu(		transitionFunc)); break; }
	case SceneId::Game:		 { currentScene = std::unique_ptr<Scene>(new Game(			transitionFunc, leaderboard, resources)); break; }
	case SceneId::Highscore: { currentScene = std::unique_ptr<Scene>(new HighscoreScene(transitionFunc, leaderboard)); break; }
	case SceneId::EndScreen: { currentScene = std::unique_ptr<Scene>(new EndScreen(		transitionFunc, leaderboard)); break; }
	}
}

void App::Update() noexcept {		// TODO Reconsider if this can really be noexcept
	inUpdate = true;
	currentScene->Update();			// TODO Consider try-catching any errors here and printing as an error, or otherwise crashing
									// Deal with it by... printing, freezing screen? going to a "something went wrong" screen?
	inUpdate = false;

	while (transitionQueued) {			// TODO Catch exceptions
		transitionQueued = false;
		ChangeTo(queuedId);
	}
}

void App::Render() const noexcept {
	BeginDrawing();				// TODO Consider moving into the Render() func for brevity
	ClearBackground(BLACK);

	inRender = true;
	currentScene->Render();
	inRender = false;

	EndDrawing();
}

// WindowLoader

WindowLoader::WindowLoader(int screenWidth, int screenHeight, const char* title)		// NOTE std::string_view doesn't work here because raylib wants a c_str pointer
{
	InitWindow(screenWidth, screenHeight, title);
}

WindowLoader::~WindowLoader()
{
	CloseWindow();
}
