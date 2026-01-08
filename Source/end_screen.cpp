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
	DrawText("PRESS ENTER TO CONTINUE", 600, 200, 40, YELLOW);

	DrawText("LEADERBOARD", 50, 100, 40, YELLOW);

	auto stats = leaderboard->GetStats();
	for (int i = 0; i < stats.size(); i++) {							// TODO Consider making into for loop
		const char* tempNameDisplay = stats[i].name.data();
		DrawText(tempNameDisplay, 50, 140 + (i * 40), 40, YELLOW);
		DrawText(TextFormat("%i", stats[i].score), 350, 140 + (i * 40), 40, YELLOW);
	}
	

}

