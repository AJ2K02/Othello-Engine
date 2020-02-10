#ifndef EVAL_HPP
#define EVAL_HPP

#include "Board.hpp"

#include <string_view>

struct Game {
	std::array<uint8_t, 60> moves;
	std::array<uint8_t, 2> scores;

	uint8_t* data() {return moves.data();}
};

/*
 * Associate a numerical value to each square of the board,
 * indicating how "good" this particular square is. For example,
 * corners have the biggest score and X-squares the lowest one.
 *
 * The score is the sum of the values of all the squares where the
 * player has a disc.
 */

// Evaluates the bitboard of a position
int scoreBitboardSquareValue(const bs64 b);

// Evaluates a position using a table of square-values
int squareValue(const Board board);

// Loads games of the game database
std::vector<Game> loadGames(const std::string_view db_file);

// Learns the values of the table by looking at a database of games
void learnValues(const std::string_view db_file);

// Updates the values for each square based on a position and the result of the game.
void updateStats(const Board& position, const uint8_t game_result);

// Divide each entry of squareValues by the corresponding one in nbOfExamples.
void averageStats();

// Calculate the border-discs feature
int borderFeature(const Board& board);

// Counts the number of available moves
int countLegalMovesFeature(const Board& board);

// Evaluation function
_score_t basic_eval(const Board& board);
#endif