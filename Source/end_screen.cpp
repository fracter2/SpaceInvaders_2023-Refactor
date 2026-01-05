#include "end_screen.h"
#include "raylib.h"

#include "common.h"

EndScreen::EndScreen(const std::function<void(SceneId)>& transitionFunc, Leaderboard& lb) noexcept
	: transitionTo(transitionFunc) 
	, leaderboard(lb)
{
	//SAVE SCORE AND UPDATE SCOREBOARD
	//newHighScore = CheckNewHighScore();
}

void EndScreen::Update() noexcept {
	//Exit endscreen										// TODO Clarify if this is a separate state from when there is a highscore
	bool newHighScore = leaderboard.CheckNewHighScore();
	if (IsKeyReleased(KEY_ENTER) && !newHighScore)
	{
		// TODO Consider SaveLeaderboard(); here
		transitionTo(SceneId::MainMenu);
		return;
	}



	if (newHighScore)										// TODO Pull out from nesting
	{
		if (CheckCollisionPointRec(GetMousePosition(), textBox)) mouseOnText = true;		// TODO Simplify, can be one line
		else mouseOnText = false;

		if (mouseOnText)									// TODO Pull out from nesting
		{
			// Set the window's cursor to the I-Beam
			SetMouseCursor(MOUSE_CURSOR_IBEAM);

			// Get char pressed on the queue
			int key = GetCharPressed();

			// Check if more characters have been pressed on the same frame
			while (key > 0)
			{
				// NOTE: Only allow keys in range [32..125]
				if ((key >= 32) && (key <= 125) && (letterCount < 9))
				{
					name[letterCount] = (char)key;
					name[letterCount + 1] = '\0'; // Add null terminator at the end of the string.
					letterCount++;
				}

				key = GetCharPressed();  // Check next character in the queue
			}

			//Remove chars 
			if (IsKeyPressed(KEY_BACKSPACE))
			{
				letterCount--;
				if (letterCount < 0) letterCount = 0;
				name[letterCount] = '\0';
			}
		}
		else SetMouseCursor(MOUSE_CURSOR_DEFAULT);

		if (mouseOnText)		// TODO This is for the textbox cursor blink animation, this should be handles by the textbox itself
		{
			framesCounter++;
		}
		else
		{
			framesCounter = 0;
		}

		// If the name is right legth and enter is pressed, exit screen by setting highscore to false and add 
		// name + score to scoreboard
		if (letterCount > 0 && letterCount < 9 && IsKeyReleased(KEY_ENTER))
		{
			std::string nameEntry(name);

			leaderboard.InsertNewHighScore(nameEntry);

			leaderboard.currentScore = 0;
		}


	}

}

void EndScreen::Render() const noexcept {
	//Code												// TODO Remove redundant comment
	//DrawText("END", 50, 50, 40, YELLOW);				// TODO Remove redundant comment
	bool newHighScore = leaderboard.CheckNewHighScore();
	if (newHighScore)
	{
		DrawText("NEW HIGHSCORE!", 600, 300, 60, YELLOW);



		// BELOW CODE IS FOR NAME INPUT RENDER
		DrawText("PLACE MOUSE OVER INPUT BOX!", 600, 400, 20, YELLOW);

		DrawRectangleRec(textBox, LIGHTGRAY);																		// TODO Make textbox into a class that can draw itself
		if (mouseOnText)
		{
			// HOVER CONFIRMIATION
			DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, RED);
		}
		else
		{
			DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);
		}

		//Draw the name being typed out
		DrawText(name, (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);

		//Draw the text explaining how many characters are used
		DrawText(TextFormat("INPUT CHARS: %i/%i", letterCount, 8), 600, 600, 20, YELLOW);

		if (mouseOnText)
		{
			if (letterCount < 9)
			{
				// Draw blinking underscore char
				if (((framesCounter / 20) % 2) == 0)
				{
					DrawText("_", (int)textBox.x + 8 + MeasureText(name, 40), (int)textBox.y + 12, 40, MAROON);
				}

			}
			else
			{
				//Name needs to be shorter
				DrawText("Press BACKSPACE to delete chars...", 600, 650, 20, YELLOW);
			}

		}

		// Explain how to continue when name is input
		if (letterCount > 0 && letterCount < 9)
		{
			DrawText("PRESS ENTER TO CONTINUE", 600, 800, 40, YELLOW);
		}

	}
	else {
		// If no highscore or name is entered, show scoreboard and call it a day
		DrawText("PRESS ENTER TO CONTINUE", 600, 200, 40, YELLOW);

		DrawText("LEADERBOARD", 50, 100, 40, YELLOW);

		auto stats = leaderboard.GetStats();

		for (int i = 0; i < stats.size(); i++)
		{
			const char* tempNameDisplay = stats[i].name.data();
			DrawText(tempNameDisplay, 50, 140 + (i * 40), 40, YELLOW);
			DrawText(TextFormat("%i", stats[i].score), 350, 140 + (i * 40), 40, YELLOW);
		}
	}

}

