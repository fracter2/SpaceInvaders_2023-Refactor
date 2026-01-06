#include "main_menu.h"
#include "raylib.h"


MainMenu::MainMenu(const std::function<void(SceneId)>& transitionFunc) noexcept
	: transitionTo(transitionFunc) 
{
}

void MainMenu::Update() noexcept {
	//Code							// TODO Remove, redudant
	if (IsKeyReleased(KEY_SPACE))
	{
		transitionTo(SceneId::Game);
		return;
	}
}

void MainMenu::Render() const noexcept {
	//Code							// TODO Remove, redudant
	DrawText("SPACE INVADERS", 200, 100, 160, YELLOW);
	DrawText("PRESS SPACE TO BEGIN", 200, 350, 40, YELLOW);
}