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

	{	// Prompt
		static constexpr int x = 600, y = 200;
		DrawText("PRESS ENTER TO CONTINUE", x, y, fontSize, YELLOW);
	}

	{	// Tittle
		static constexpr int x = 50, y = 100;
		DrawText("LEADERBOARD", x, y, fontSize, YELLOW);
	}

	for (int row = 0; const auto& entry : leaderboard->GetStats()) {
		static constexpr int offsetY = 140;
		static constexpr int rowHeight = 40;
		const int y = offsetY + (row * rowHeight);

		static constexpr int nameX = 50;
		static constexpr int scoreX = 350;
		DrawText(entry.name.data(),				nameX,  y, fontSize, YELLOW);		// TODO Merge into one drawText using std::format (alignment)
		DrawText(TextFormat("%i", entry.score), scoreX, y, fontSize, YELLOW);

		row++;
	}
}

