#include "highscore_scene.h"

// Util
bool isValidKey(int key) noexcept {
	return (key >= 32) && (key <= 125);		// TODO Clarify these numbers!!!
}

void HighscoreScene::ParseNameInput() noexcept {
	int key = GetCharPressed();
	while (key > 0) {
		if (isValidKey(key) && (name.size() < maxNameLength)) {
			name.push_back((char)key);
		}

		key = GetCharPressed();  // Check next character in the queue
	}

	if (IsKeyPressed(KEY_BACKSPACE) && name.size() > 0) {
		name.pop_back();
	}
}

// Implementation
HighscoreScene::HighscoreScene(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb) noexcept 
	: transitionTo(transitionFunc)
	, leaderboard(&lb)
{
	if (!leaderboard->CheckNewHighScore()) {
		transitionTo(SceneId::EndScreen);
	}
}

void HighscoreScene::Update() noexcept
{
	if (name.size() > 0 && IsKeyReleased(KEY_ENTER)) {
		leaderboard->InsertNewHighScore(name);
		leaderboard->currentScore = 0;
		transitionTo(SceneId::EndScreen);
		return;
	}

	mouseOnText = CheckCollisionPointRec(GetMousePosition(), textBox);		// TODO Consider a dedicated cursorBlinkAnimation class
	if (mouseOnText) {
		framesCounter++;
		SetMouseCursor(MOUSE_CURSOR_IBEAM);
		ParseNameInput();
	}
	else {
		framesCounter = 0;
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);
	}
}

void HighscoreScene::Render() const noexcept
{
	static constexpr int fontsizeLarge = 60;
	static constexpr int fontsizeMedium = 40;
	static constexpr int fontsizeSmall = 20;
	DrawText("NEW HIGHSCORE!", 600, 300, fontsizeLarge, YELLOW);					// TODO Clarify constants

	// NAME INPUT TEXT-BOX
	DrawText("PLACE MOUSE OVER INPUT BOX!", 600, 400, fontsizeSmall, YELLOW);		// TODO Clarify constants

	DrawRectangleRec(textBox, LIGHTGRAY);								// TODO Make textbox into a class that can draw itself

	static constexpr float lineThickness = 1;
	DrawRectangleLinesEx(textBox, lineThickness, mouseOnText ? RED : DARKGRAY);

	DrawText(name.c_str(), (int)textBox.x + 5, (int)textBox.y + 8, fontsizeMedium, MAROON);									// TODO Clarify constants

	DrawText(TextFormat("INPUT CHARS: %i/%i", name.size(), 8), 600, 600, fontsizeSmall, YELLOW);							// TODO Clarify constants

	if (mouseOnText) {													// TODO Merge with above if() if possible
		if (name.size() < maxNameLength) {
			// Draw blinking underscore char
			if (((framesCounter / 20) % 2) == 0) {						// TODO Move to private func
				DrawText("_", (int)textBox.x + 8 + MeasureText(name.c_str(), 40), (int)textBox.y + 12, fontsizeMedium, MAROON);	// TODO Consider DrawTextEx() to remove casts
			}
		}
		else {
			// Name needs to be shorter
			DrawText("Press BACKSPACE to delete chars...", 600, 650, fontsizeSmall, YELLOW);								// TODO Clarify constants
		}

	}

	if (name.size() > 0 && name.size() < maxNameLength) {							// TODO Name constatn max length
		DrawText("PRESS ENTER TO CONTINUE", 600, 800, fontsizeMedium, YELLOW);		// TODO Clarify constants
	}

	
}
