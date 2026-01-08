#include "end_screen.h"
#include "raylib.h"

#include "common.h"

EndScreen::EndScreen(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb) noexcept
	: transitionTo(transitionFunc) 
	, leaderboard(&lb)
{
	//SAVE SCORE AND UPDATE SCOREBOARD
	//newHighScore = CheckNewHighScore();
}

void EndScreen::Update() noexcept {									
	bool newHighScore = leaderboard->CheckNewHighScore();

	if (!newHighScore) {													// TODO Clarify if this is a separate state from when there is a highscore
		if (IsKeyReleased(KEY_ENTER)) {
			transitionTo(SceneId::MainMenu);
		}

		return;
	}

	mouseOnText = CheckCollisionPointRec(GetMousePosition(), textBox);		// TODO Consider a dedicated cursorBlinkAnimation class

	if (mouseOnText) {														// TODO Pull out from nesting
		framesCounter++;
		SetMouseCursor(MOUSE_CURSOR_IBEAM);

		int key = GetCharPressed();
		while (key > 0) {
			// NOTE: Only allow keys in range [32..125]
			if ((key >= 32) && (key <= 125) && (name.size() < 9)) {			// TODO Make this check a func
				name.push_back((char)key);
			}

			key = GetCharPressed();  // Check next character in the queue
		}

		if (IsKeyPressed(KEY_BACKSPACE) && name.size() > 0) {
			name.pop_back();
		}
	}
	else { 
		framesCounter = 0;
		SetMouseCursor(MOUSE_CURSOR_DEFAULT); 
	}

	if (name.size() > 0 && IsKeyReleased(KEY_ENTER)) {
		leaderboard->InsertNewHighScore(name);
		leaderboard->currentScore = 0;
	}
}

void EndScreen::Render() const noexcept {
	bool newHighScore = leaderboard->CheckNewHighScore();
	if (newHighScore) {
		DrawText("NEW HIGHSCORE!", 600, 300, 60, YELLOW);

		// NAME INPUT TEXT-BOX
		DrawText("PLACE MOUSE OVER INPUT BOX!", 600, 400, 20, YELLOW);

		DrawRectangleRec(textBox, LIGHTGRAY);								// TODO Make textbox into a class that can draw itself
		if (mouseOnText) {
			// HOVER CONFIRMIATION
			DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, RED);	
		}
		else {
			DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);
		}

		DrawText(name.c_str(), (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);

		DrawText(TextFormat("INPUT CHARS: %i/%i", name.size(), 8), 600, 600, 20, YELLOW);

		if (mouseOnText) {
			if (name.size() < 9) {
				// Draw blinking underscore char
				if (((framesCounter / 20) % 2) == 0) {
					DrawText("_", (int)textBox.x + 8 + MeasureText(name.c_str(), 40), (int)textBox.y + 12, 40, MAROON);
				}

			}
			else {
				// Name needs to be shorter
				DrawText("Press BACKSPACE to delete chars...", 600, 650, 20, YELLOW);
			}

		}

		// Explain how to continue when name is input
		if (name.size() > 0 && name.size() < 9) {
			DrawText("PRESS ENTER TO CONTINUE", 600, 800, 40, YELLOW);
		}

	}
	else {
		// If no highscore or name is entered, show scoreboard and call it a day
		DrawText("PRESS ENTER TO CONTINUE", 600, 200, 40, YELLOW);

		DrawText("LEADERBOARD", 50, 100, 40, YELLOW);

		auto stats = leaderboard->GetStats();

		for (int i = 0; i < stats.size(); i++)
		{
			const char* tempNameDisplay = stats[i].name.data();
			DrawText(tempNameDisplay, 50, 140 + (i * 40), 40, YELLOW);
			DrawText(TextFormat("%i", stats[i].score), 350, 140 + (i * 40), 40, YELLOW);
		}
	}

}

