#include "leaderboard.h"
#include "gsl/assert"

bool Leaderboard::IsNewHighscore() const noexcept 
{
	if (stats.empty() || currentScore > stats.back().score)	{ 
		return true; 
	}

	return false;
}

// NOTE Resets current score afterwards. 
void Leaderboard::SubmitCurrentScore(std::string_view name) noexcept 
{
	for (auto it = stats.begin(); it != stats.end(); ++it) {		// Closer to usage at .insert()
		if (currentScore <= (*it).score) { continue; }				// De-nested if()... could re-nest to avoid "continue" and "break"

		if (stats.size() >= maxSize) {								// Clarify popping intent / fix
			stats.pop_back(); 
		}
		stats.insert(it, { name, currentScore });
		break;
	}

	ResetScore();
	// TODO GSL Ensures current score == 0
}

Leaderboard::Leaderboard(const std::vector<Entry>& stats) noexcept
	: stats(stats)
{
}

Leaderboard::Leaderboard(std::vector<Entry>&& stats) noexcept
	: stats(stats)
{
}

Leaderboard::Entry::Entry(std::string_view name, int score) noexcept
	: name(name)
	, score(score)
{
}

Leaderboard GetExampleLeaderboard()
{
	return Leaderboard({ {"Player 1", 500}, { "Player 2", 400 }, { "Player 3", 300 }, { "Player 4", 200 }, { "Player 5", 100 } });
}

