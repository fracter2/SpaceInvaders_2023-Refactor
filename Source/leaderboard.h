#pragma once
#include <string>
#include <string_view>
#include <vector>


class Leaderboard {
public:
	struct Entry {
		Entry(std::string_view name, int score) noexcept;
		std::string name;
		int score;
	};

	int currentScore = 0;						// TODO Refactor away, make funcs use args instead (Doesn't make sense to keep this here, needs to be kept track off)
	[[nodiscard("why get if u don use? hm?")]] const std::vector<Entry>& const GetStats() const noexcept { return stats; }

	bool IsNewHighscore() const noexcept;
	void InsertNewHighScore(std::string_view name) noexcept;

	static constexpr size_t maxSize = 5;

private:
	// TODO Consider replacing with std::inplace_vector
	// TODO Move stats initialization to some "example" or "fake player scores"  
	// to separate implementation details from filler
	std::vector<Entry> stats = { {"Player 1", 500}, {"Player 2", 400}, {"Player 3", 300}, {"Player 4", 200}, {"Player 5", 100} };
};