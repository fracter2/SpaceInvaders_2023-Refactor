#include "main_menu.h"
#include "raylib.h"

MainMenu::MainMenu(const std::function<void(SceneId)>& transitionFunc) noexcept
	: transitionTo(transitionFunc) 
{
}

void MainMenu::Update() noexcept 
{
	if (IsKeyReleased(KEY_SPACE)) {
		transitionTo(SceneId::Game);
		return;
	}
}

void MainMenu::Render() const noexcept 
{
	DrawText("SPACE INVADERS", 200, 100, 160, YELLOW);				// TODO Clarify constexpr 
	DrawText("PRESS SPACE TO BEGIN", 200, 350, 40, YELLOW);
}