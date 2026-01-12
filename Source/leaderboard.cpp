#include "leaderboard.h"
#include <fstream>
#include <iostream>
#include <cassert>


bool Leaderboard::IsNewHighscore() const noexcept {
	if (stats.empty() || currentScore > stats.back().score)	{ 
		return true; 
	}

	return false;
}

// NOTE Resets current score afterwards. 
void Leaderboard::SubmitCurrentScore(std::string_view name) noexcept {
	Entry newData = { name, currentScore };

	for (auto it = stats.begin(); it != stats.end(); ++it) {
		if (newData.score <= (*it).score) { continue; }

		if (stats.size() >= maxSize) { 
			stats.pop_back(); 
		}
		stats.insert(it, newData);
		break;
	}

	ResetScore();
}

Leaderboard::Entry::Entry(std::string_view name, int score) noexcept
	: name(name)
	, score(score)
{
}
