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

	static constexpr int nameX = 50;
	static constexpr int scoreX = 350;
	static constexpr int y = 140;
	static constexpr int rowHeight = 40;
	const auto& stats = leaderboard->GetStats();
	for (int row = 0; row < stats.size(); row++) {
		DrawText(stats[row].name.data(),			 nameX,  y + (row * rowHeight), fontSize, YELLOW);
		DrawText(TextFormat("%i", stats[row].score), scoreX, y + (row * rowHeight), fontSize, YELLOW);
		row++;
	}
}

