
#include "Board.hpp"
#include "Eval.hpp"
#include "Utils.hpp"

#include <array>
#include <fstream>
#include <string_view>

std::array<double, 64> squareValues;
std::array<unsigned long, 64> nbOfExamples;

int scoreBitboardSquareValue(const bs64 b) {
	static thread_local constexpr std::array<int, 64> SquareVals {
		20, -1, 3, 4, 4, 3, -1, 20,
		-1,-15, 2, 2, 2, 2,-15, -1,
		 3,  2, 3, 3, 3, 3,  2,  3,
		 4,  2, 3, 5, 5, 3,  2,  4,
		 4,  2, 3, 5, 5, 3,  2,  4,
		 3,  2, 3, 3, 3, 3,  2,  3,
		-1,-15, 2, 2, 2, 2,-15, -1,
		20, -1, 3, 4, 4, 3, -1, 20
	}; 
	int score = 0;
	for (unsigned i = 0; i < 64; ++i) {
		if (b >> i & 1)
			score += SquareVals[63-i];
	}
	return score;
}

int squareValue(const Board board) {
	// define which bitboard represents the player / the opponent
	//const bs64& player_discs = board.player ? board.black : board.white;
	//const bs64& opp_discs = (!board.player) ? board.black : board.white;
	const bs64& player_discs = board.black;
	const bs64& opp_discs = board.white;

	// rate these bitboards using square value
	const int player_score = scoreBitboardSquareValue(player_discs);
	const int opp_score = scoreBitboardSquareValue(opp_discs);
	//std::cout << "Debug : " << player_score << " " << opp_score << std::endl;

	// the total score is the difference of the player's score minus the score of the opponent
	return player_score - opp_score;
}

std::vector<Game> loadGames(const std::string_view db_file) {
	std::vector<Game> games;

	//Read games from the file
	std::ifstream file;
	file.open(std::string(db_file), std::ifstream::binary);
	if (file.is_open()) {
		//Read the number of games contained in the file.
		uint32_t nb_of_games;
		file.read(reinterpret_cast<char*>(&nb_of_games), 4);

		games.resize(nb_of_games);
		uint16_t players_nb[2];
		for (unsigned i = 0; i < nb_of_games; ++i) {
			file.read(reinterpret_cast<char*>(players_nb), 4);
			file.read(reinterpret_cast<char*>(games[i].scores.data()), 2);
			file.read(reinterpret_cast<char*>(games[i].data()), 60);
		}
		std::cout << "Loaded all " << nb_of_games << " games." << std::endl; 
	} else {
		std::cerr << "Failed to open file in learnValues." << std::endl;
	}
	return games;
}


void learnValues(const std::string_view db_file) {
	std::vector<Game> games = loadGames(db_file);
	squareValues.fill(0);
	nbOfExamples.fill(0);
	Board position = Board::StartingPosition();
	unsigned t = 0;
	for (const Game& game : games) {
		/*
		- for each position
		- update stats
		*/
		uint8_t game_result = game.scores[0];
		int move = 0;
		for (auto m : game.moves) {
			++move;
			if (!position.canPlay()) {
				/*
				If the player to play has no moves, he passes.
				Then, we check whether the other player can play or not
				(if not, the game is over and we break out of the loop).
				*/
				position.pass();
				if (!position.canPlay())
					break;
			} else {
				Move next_move = position.createMoveFromIndex(fromWthorCoords(m));
				position.apply_move(next_move);
				if (move > 10 && move < 30) {
					updateStats(position, game_result);
					t++;
				}
			}
		}
		// Reset the position for the next game.
		position = Board::StartingPosition();
	}
	averageStats();
	std::cout << t << std::endl;
	printCtn(squareValues);
	printCtn(nbOfExamples);
}

void updateStats(const Board& position, const uint8_t game_result) {
	// TODO a game_result of 0 (draw) is hereby considered as a white win...
	bool black_won = game_result > 0;
	for (unsigned i = 0; i < 64; ++i) {
		if ((position.black >> (63-i)) & 1) {
			// If there is a black disc at the position i
			++nbOfExamples[i];
			squareValues[i] += black_won;
		} else if ((position.white >> (63-i)) & 1) {
			// If there is a white disc at the position i
			++nbOfExamples[i];
			squareValues[i] -= black_won;
		}
	}
}

void averageStats() {
	for (unsigned i = 0; i < 64; ++i) {
		// nbOfExamples / 10 allows to avoid rounding floating point numbers too much when convertion to int
		squareValues[i] *= nbOfExamples[i] != 0 ? 10.0/nbOfExamples[i] : 0; 
	}
}

int borderFeature(const Board& board) {
	// player's borders and other's borders
	bs64 pborders(0);
	bs64 oborders(0);
	// bitmask allowing to remove edges
	constexpr bs64 NOT_EDGES = ~0xFF818181818181FF;
	const bs64 player_discs = board.black & NOT_EDGES;
	const bs64 opp_discs = board.white & NOT_EDGES;
	bs64 empties = ~(player_discs | opp_discs);

	//add border discs in each direction
	pborders |= (player_discs & (empties >> 1));
	oborders |= (opp_discs & (empties >> 1));
	pborders |= (player_discs & (empties << 1));
	oborders |= (opp_discs & (empties << 1));
	pborders |= (player_discs & (empties >> 8));
	oborders |= (opp_discs & (empties >> 8));
	pborders |= (player_discs & (empties << 8));
	oborders |= (opp_discs & (empties << 8));
	return popcnt(oborders) - popcnt(pborders);
}

int countLegalMovesFeature(const Board& board) {
	std::array<bs64, 8> legal_moves = board.gen_moves();
	bs64 merged { 0 };
	for (const bs64 m : legal_moves) {
		merged |= m;
	}
	// returns +/- popcnt(merged) depending on the color
	return (int(board.player) * 2 - 1) * popcnt(merged);
}


_score_t basic_eval(const Board& board) {
	const int square_value_score = squareValue(board);
	const int borders_score = borderFeature(board);
	const int nb_moves_score = countLegalMovesFeature(board);
	// weights associated to the different features
	constexpr int alpha = 3;
	constexpr int beta = 2;
	return square_value_score + alpha * borders_score + beta * nb_moves_score + 0.1;
}
