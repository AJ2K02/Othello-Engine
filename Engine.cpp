#include <chrono>
#include <functional>
#include <iostream>

#include "BetterEval.hpp"
#include "Board.hpp"
#include "Config.hpp"
#include "Eval.hpp"
#include "IO.hpp"
#include "PatternEval.hpp"
#include "Utils.hpp"

#include "Search.hpp"

int main() {

	Board b = move_seq2board("c4e3f3c5d3f4d6c6f5d2c3e6f6e2f2");

	bool calculate_params = 0;
	bool calculate_weights = 0;
	std::vector<Game> games;
	Evaluator e;
	if (calculate_params) {
		games = loadGames("./python/GamesTop200.bin");
		DEBUG_MSG("Loaded games");
		e.learnGames(games, 0, 60);
		DEBUG_MSG("Learned games");
		e.save("data/v2coeff.bin");
	} else {
		e.load("data/v2coeff.bin");
	}

	if (calculate_weights) {
		if (!calculate_params)
			games = loadGames("./python/GamesTop200.bin");
		e.learnWeights(games);
		DEBUG_MSG("Learned weights");
		e.save("data/v2coeff.bin");
		printCtn(e.getWeights());
		DEBUG_MSG(e.getBias());
	}

	auto ev = [&e](const Board& board) { return e.evalPosition(board); };
	Interpreter(b).setEvalFunc(ev).run();

	return 0;
}


/*
TODO : delete memory allocated by minimax_ab / use smart pointers
*/