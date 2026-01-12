#include "end_screen.h"
#include "raylib.h"
#include "common.h"


EndScreen::EndScreen(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb) noexcept
	: transitionTo(transitionFunc) 
	, leaderboard(&lb)
{
	if (leaderboard->IsNewHighscore()) {		// TODO Should this be an assert? to enfore only one correct order-of-transitions
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
		// NOTE The cpp guidenline ES.10 states to declare only one var/const per line. 
		// However i have opted to allow cases like this. It is only two, inherently 
		// simple x / y that MUST be initialized (constexpr). It also aligns well with 
		// the following line. 
		static constexpr int x = 600, y = 200;
		DrawText("PRESS ENTER TO CONTINUE", x, y, fontSize, YELLOW);
	}

	{	// Tittle
		static constexpr int x = 50, y = 100;
		DrawText("LEADERBOARD", x, y, fontSize, YELLOW);
	}

	// Score list
	for (int row = 0; const auto& entry : leaderboard->GetStats()) {
		static constexpr int offsetY = 140, rowHeight = 40;
		const int y = offsetY + (row * rowHeight);

		static constexpr int nameX = 50, scoreX = 350;
		DrawText(entry.name.data(),				nameX,  y, fontSize, YELLOW);		// NOTE These cannot be merged well-enough using std::format because
		DrawText(TextFormat("%i", entry.score), scoreX, y, fontSize, YELLOW);		// the filler doesn't take into account letter-width differences

		row++;
	}
}

