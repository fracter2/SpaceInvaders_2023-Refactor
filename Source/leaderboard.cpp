#include "leaderboard.h"
#include <fstream>
#include <iostream>

bool Leaderboard::IsNewHighscore() const {
	if (stats.empty())						{ return true; }
	if (currentScore > stats.back().score)	{ return true; }

	return false;
}

void Leaderboard::InsertNewHighScore(std::string name) {		// TODO Consider string_view
	Entry newData = { name, currentScore };

	for (int i = 0; i < stats.size(); i++) {						// TODO Consider replacing with an algorithm w lambda
		if (newData.score > stats[i].score) {

			stats.insert(stats.begin() + i, newData);			// Can this be done in an for-each loop? TRY

			stats.pop_back();									// TODO Consider removing. is this to limit size? then use an if(). Unclear.

			i = stats.size();									// TODO Replace with return

		}
	}
}

void Leaderboard::LoadLeaderboard() {							// TODO Consider removing (unused) Move to separate file along with SaveLeaderboard()
	// CLEAR LEADERBOARD

	// OPEN FILE

	// READ DATA

	// WRITE DATA ONTO LEADERBOARD

	//CLOSE FILE
}

void Leaderboard::SaveLeaderboard() {							// TODO Consider removing, used once, not fully implemented yet
	// SAVE LEADERBOARD AS ARRAY

	// OPEN FILE
	std::fstream file;

	file.open("Leaderboard");

	if (!file) {
		std::cout << "file not found \n";

	}
	else {
		std::cout << "file found \n";
	}
	// CLEAR FILE

	// WRITE ARRAY DATA INTO FILE

	// CLOSE FILE
}

Leaderboard::Entry::Entry(std::string_view name, int score) noexcept
	: name(name)
	, score(score)
{
}
