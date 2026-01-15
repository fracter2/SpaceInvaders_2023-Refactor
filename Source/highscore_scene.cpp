#include "highscore_scene.h"


// Keep helper funcs inside an anonymous namespace. SF.22
namespace 
{	
	bool IsValidKey(int key) noexcept 
	{
		return (key >= 32) && (key <= 125);		// TODO Clarify these numbers!!!
	}

	void ParseNameInput(std::string& name, int maxLength) noexcept 
	{
		int key = GetCharPressed();				// TODO TRY moving intisde while(*here*) ES.6
		while (key > 0) {
			if (IsValidKey(key) && (name.size() < maxLength)) {
				name.push_back((char)key);		// TODO Either gsl::narrow_cast or static_cast or make a wrapper "GetCharPressed()" ES.46, ES.49
			}

			key = GetCharPressed();  // Check next character in the queue
		}

		if (IsKeyPressed(KEY_BACKSPACE) && name.size() > 0) {
			name.pop_back();
		}
	}

	void RenderBlinkingUnderscore(const char* cString, int x, int y, int fontSize, int framesCounter) noexcept
	{
		static constexpr int duration = 20;
		if ((framesCounter / duration) % 2) {						// NOTE "% 2" always gives 0 or 1, which is same as false / true
			const int offsetX = MeasureText(cString, fontSize);
			DrawText("_", x + offsetX, y, fontSize, MAROON);
		}
	}
}

// HighscoreScene

HighscoreScene::HighscoreScene(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb) noexcept 
	: transitionTo(transitionFunc)
	, leaderboard(&lb)
{
	if (!leaderboard->IsNewHighscore()) {
		transitionTo(SceneId::EndScreen);
	}
}

void HighscoreScene::Update() noexcept
{
	if (name.size() > 0 && IsKeyReleased(KEY_ENTER)) {
		leaderboard->SubmitCurrentScore(name);
		transitionTo(SceneId::EndScreen);
		return;
	}

	mouseOnText = CheckCollisionPointRec(GetMousePosition(), textBox);		// TODO Consider a dedicated cursorBlinkAnimation class
	if (mouseOnText) {
		framesCounter++;
		SetMouseCursor(MOUSE_CURSOR_IBEAM);
		ParseNameInput(name, maxNameLength);
	}
	else {
		framesCounter = 0;
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);
	}
}

void HighscoreScene::Render() const noexcept			// TODO Consider making a simple UIBox class that can handle aligning positions like here
{
	static constexpr int fontsizeLarge = 60;
	static constexpr int fontsizeMedium = 40;
	static constexpr int fontsizeSmall = 20;
	static constexpr int posX = static_cast<int>(textBox.x);
	static constexpr int posY = static_cast<int>(textBox.y);

	DrawText("NEW HIGHSCORE!",				posX, posY - 200, fontsizeLarge, YELLOW);
	DrawText("PLACE MOUSE OVER INPUT BOX!", posX, posY - 100, fontsizeSmall, YELLOW);

	// NAME INPUT TEXT-BOX
	DrawRectangleRec(textBox, LIGHTGRAY);

	static constexpr float lineThickness = 1;
	DrawRectangleLinesEx(textBox, lineThickness, mouseOnText ? RED : DARKGRAY);
	DrawText(name.c_str(), posX + 5, posY + 8, fontsizeMedium, MAROON);

	if (mouseOnText) {
		if (name.size() < maxNameLength) {
			RenderBlinkingUnderscore(name.c_str(), posX + 8, posY + 12, fontsizeMedium, framesCounter); 
		}
		else { 
			DrawText("Press BACKSPACE to delete chars...", posX, posY + 150, fontsizeSmall, YELLOW); 
		}
	}

	DrawText(TextFormat("INPUT CHARS: %i/%i", name.size(), maxNameLength - 1), posX, posY + 100, fontsizeSmall, YELLOW);

	if (name.size() > 0 && name.size() < maxNameLength) {
		DrawText("PRESS ENTER TO CONTINUE", posX, posY + 300, fontsizeMedium, YELLOW);
	}
}
