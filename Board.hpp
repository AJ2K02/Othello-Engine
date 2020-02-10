#ifndef BOARD_HPP
#define BOARD_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "Config.hpp"

#define PASS_MOVE_INDEX 255

struct Move {
	Directions dir = 0;
	MoveIndex move;
};

struct Board {
	static Board StartingPosition();

	bs64 black;
	bs64 white;
	bool player;

	// Generates a list of all legal moves in the form of 8 bitboards (one per direction)
	std::array<bs64, 8> gen_moves() const;
	// Generates a list of all legal moves in the form of a vector of Move.
	std::vector<Move> gen_move_list(const std::array<bs64, 8>& moves);
	// Creates a Move object from the index of the square of the play.
	Move createMoveFromIndex(const int index) const;
	// Finds the discs turned for one move m
	bs64 discs_to_turn(const Move m);
	// Play a move knowing the discs to turn.
	void apply_move(bs64 turned, MoveIndex mi);
	// Play a move m.
	void apply_move(const Move m);
	void apply_move(const int m);
	// Pass
	void pass();
	// Returns a boolean indicating whether the player must pass or not.
	[[nodiscard]] bool canPlay() const;
	// Returns 0 if no one can play, 
	//         1 if the player can play
	//         2 if the player must pass 
	[[nodiscard]] int hasLegalMoves() const;
	[[nodiscard]] std::array<uint8_t, 64> toIntArray() const;

	//Returns the difference between border discs (used for evaluating positions)
	int getBorderDiscsScore();

	void print();
};

Board str2board(const std::string& str, bool to_play=1);
Board move_seq2board(std::string str);

/*
 * Exception thrown when a move has an index smaller than 0 or 
 * bigger than 63 - so a move "out of the board".
 */ 
class OutOfBoardMove : public std::exception {
	public:
	OutOfBoardMove() = default;
	OutOfBoardMove(const int move) : m_move(move) {}

	int getMove() const { return m_move; }

	private:
	int m_move;
};

#endif