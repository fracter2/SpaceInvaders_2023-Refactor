#pragma once
#include <string>
#include <vector>

struct FinishedGameScore		// TODO Consider renaming to "GameScore" or similar
{
	std::string name;
	int score;
};


class Leaderboard {

	// TODO Move stats initialization to some "example" or "fake player scores"  
	// to separate implementation details from filler
	// TODO Move to a private: under public:
	std::vector<FinishedGameScore> stats = { {"Player 1", 500}, {"Player 2", 400}, {"Player 3", 300}, {"Player 4", 200}, {"Player 5", 100} };

public:
	int currentScore = 0;						// TODO Refactor away, make funcs use args instead (Doesn't make sense to keep this here, needs to be kept track off)
	const std::vector<FinishedGameScore>& const GetStats() const noexcept {	return stats; } // TODO Add a nodiscard

	bool IsNewHighscore() const;				// TODO Rename to clarify, like "isHighScore()" or "justBeatHighScore()"

	void InsertNewHighScore(std::string name);	// TODO Consider using string_view

	void LoadLeaderboard();						// TODO Consider implementing or removing (unused)
	void SaveLeaderboard();						// TODO Finish implementing this or remove (used but incomplete)

};