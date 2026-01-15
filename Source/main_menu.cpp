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
	{
		static constexpr int fontsize = 160;
		static constexpr int x = 200, y = 100;
		DrawText("SPACE INVADERS", x, y, fontsize, YELLOW);
	}
	{
		static constexpr int fontsize = 40;
		static constexpr int x = 200, y = 350;
		DrawText("PRESS SPACE TO BEGIN", x, y, fontsize, YELLOW);
	}
}