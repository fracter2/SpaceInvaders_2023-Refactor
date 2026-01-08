#include "end_screen.h"
#include "raylib.h"

#include "common.h"

EndScreen::EndScreen(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb) noexcept
	: transitionTo(transitionFunc) 
	, leaderboard(&lb)
{
	if (leaderboard->CheckNewHighScore()) {
		transitionTo(SceneId::Highscore);
	}
}

void EndScreen::Update() noexcept {									
	if (IsKeyReleased(KEY_ENTER)) {
		transitionTo(SceneId::MainMenu);
	}
}

void EndScreen::Render() const noexcept {
	static constexpr int fontSize = 40;

	static constexpr int promptX = 600;
	static constexpr int promptY = 200;
	DrawText("PRESS ENTER TO CONTINUE", promptX, promptY, fontSize, YELLOW);

	static constexpr int titleX = 50;
	static constexpr int titleY = 100;
	DrawText("LEADERBOARD",				titleX,	 titleY,  fontSize, YELLOW);

	for (int row = 0; const auto& entry : leaderboard->GetStats()) {
		static constexpr int y = 140;
		static constexpr int rowHeight = 40;

		static constexpr int nameX = 50;
		DrawText(entry.name.data(),				nameX, y + (row * rowHeight), fontSize, YELLOW);

		static constexpr int scoreX = 350;
		DrawText(TextFormat("%i", entry.score), scoreX, y + (row * rowHeight), fontSize, YELLOW);
		row++;
	}
}

