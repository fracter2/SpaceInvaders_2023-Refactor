#include "leaderboard.h"
#include <fstream>
#include <iostream>
#include <cassert>


bool Leaderboard::IsNewHighscore() const {
	if (stats.empty())						{ return true; }
	if (currentScore > stats.back().score)	{ return true; }

	return false;
}

void Leaderboard::InsertNewHighScore(std::string_view name) {
	Entry newData = { name, currentScore };

	for (auto it = stats.begin(); it != stats.end(); ++it) {
		if (newData.score > (*it).score) {
			if (stats.size() >= maxSize) { 
				stats.pop_back(); 
			}
			stats.insert(it, newData);				// Can this be done in an for-each loop? TRY
			break;
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
