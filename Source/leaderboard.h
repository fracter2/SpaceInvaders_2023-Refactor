#pragma once		// TODO Consider making into #ifdef... type include guard (portability, recomended by cpp guidelines)
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

	explicit Leaderboard(const std::vector<Entry>& stats) noexcept;
	explicit Leaderboard(std::vector<Entry>&& stats) noexcept;

	bool IsNewHighscore() const noexcept;
	void SubmitCurrentScore(std::string_view name) noexcept;
	void AddScore(int score) noexcept { currentScore += score; }
	void ResetScore() noexcept { currentScore = 0; }
	[[nodiscard("why get if u don use? hm?")]] int GetScore() const noexcept { return currentScore; }
	[[nodiscard("why get if u don use? hm?")]] const std::vector<Entry>& GetStats() const noexcept { return stats; }

	static constexpr size_t maxSize = 5;

private:
	int currentScore = 0;

	// TODO Consider replacing with std::inplace_vector
	// TODO Move stats initialization to some "example" or "fake player scores"  
	// to separate implementation details from filler
	std::vector<Entry> stats;
};

[[nodiscard]] Leaderboard GetExampleLeaderboard();