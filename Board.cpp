#include "Board.hpp"
#include "Config.hpp"
#include "Utils.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <locale>
#include <string>
#include <vector>

#define NL std::cout<<std::endl

#define RIGHT_MASK 0xfefefefefefefefe
#define LEFT_MASK 0x7f7f7f7f7f7f7f7f
#define UP_MASK 0x00ffffffffffffff
#define DOWN_MASK 0xffffffffffffff00
#define UP_RIGHT_MASK (UP_MASK & RIGHT_MASK)
#define DOWN_RIGHT_MASK (DOWN_MASK & RIGHT_MASK)
#define DOWN_LEFT_MASK (DOWN_MASK & LEFT_MASK)
#define UP_LEFT_MASK (UP_MASK & LEFT_MASK)


Board Board::StartingPosition() {
	Board pos;
	pos.white = (bs64(1) << 36) | (bs64(1) << 27);
	pos.black = (bs64(1) << 35) | (bs64(1) << 28);
	pos.player = true;
	return pos;
}

std::array<bs64, 8> Board::gen_moves() const {
	const bs64 own = player ? black : white;
	const bs64 opp = player ? white : black;
	bs64 emp = ~(own | opp);
	std::array<bs64, 8> moves {0, 0, 0, 0, 0, 0, 0, 0};

	bs64 t = (own << 1) & RIGHT_MASK;
	while (t != 0) {
		t = ((t & opp) << 1) & RIGHT_MASK;
		moves[0] |= t & emp;
	}

	t = (own >> 1) & LEFT_MASK;
	while (t != 0) {
		t = ((t & opp) >> 1) & LEFT_MASK;
		moves[1] |= t & emp;
	}

	t = (own >> 8) & UP_MASK;
	while (t != 0) {
		t = ((t & opp) >> 8) & UP_MASK;
		moves[2] |= t & emp;
	}

	t = (own << 8) & DOWN_MASK;
	while (t != 0) {
		t = ((t & opp) << 8) & DOWN_MASK;
		moves[3] |= t & emp;
	}

	t = (own >> 7) & UP_RIGHT_MASK;
	while (t != 0) {
		t = ((t & opp) >> 7) & UP_RIGHT_MASK;
		moves[4] |= t & emp;
	}

	t = (own << 9) & DOWN_RIGHT_MASK;
	while (t != 0) {
		t = ((t & opp) << 9) & DOWN_RIGHT_MASK;
		moves[5] |= t & emp;
	}

	t = (own << 7) & DOWN_LEFT_MASK;
	while (t != 0) {
		t = ((t & opp) << 7) & DOWN_LEFT_MASK;
		moves[6] |= t & emp;
	}

	t = (own >> 9) & UP_LEFT_MASK;
	while (t != 0) {
		t = ((t & opp) >> 9) & UP_LEFT_MASK;
		moves[7] |= t & emp;
	}
	return moves;
}

std::vector<Move> Board::gen_move_list(const std::array<bs64, 8>& moves) {
	//TODO : when iterating over squares check if current square is actually empty

	std::vector<Move> ml;
	// iterate over all squares
	for (unsigned i = 0; i < 64; ++i) {
		Move m;
		// iterate over all directions
		for (unsigned j = 0; j < 8; ++j) {
			m.dir |= ((moves[j] & (bs64(1) << i)) != 0) << j;
		}
		// add move to list if it turns discs in at least one direction
		if (m.dir != 0) {
			m.move = i;
			ml.push_back(m);
		}
	}
	return ml;
}

Move Board::createMoveFromIndex(const int index) const {
	Move m;
	m.move = index;
	bs64 mbb = bs64(1) << m.move;
	std::array<bs64, 8> legals = this->gen_moves();
	for (unsigned d = 0; d < 8; ++d) {
		if (mbb & legals[d]) {
			m.dir |= 1 << d;
		}
	}
	return m;
}

bs64 Board::discs_to_turn(const Move m) {
	const bs64 opp = player ? white : black;
	bs64 mbb = bs64(1) << m.move;
	bs64 temp = mbb;
	bs64 tt = 0; // to turn
	for (unsigned d = 0; d < 8; ++d) {
		if ((m.dir & bs64(1) << d) != 0) {
			switch (d) {
				case 0:
				while (((temp >>= 1) & opp) != 0)
					tt |= temp;
				break;

				case 1:
				while (((temp <<= 1) & opp) != 0)
					tt |= temp;
				break;

				case 2:
				while (((temp <<= 8) & opp) != 0)
					tt |= temp;
				break;

				case 3:
				while (((temp >>= 8) & opp) != 0)
					tt |= temp;
				break;

				case 4:
				while (((temp <<= 7) & opp) != 0)
					tt |= temp;
				break;

				case 5:
				while (((temp >>= 9) & opp) != 0)
					tt |= temp;
				break;

				case 6:
				while (((temp >>= 7) & opp) != 0)
					tt |= temp;
				break;

				case 7:
				while (((temp <<= 9) & opp) != 0)
					tt |= temp;
				break;
			}
			temp = mbb;
		}
	}
	return tt;
}

void Board::apply_move(bs64 turned, MoveIndex mi) {
		bs64& own = player ? black : white;
		bs64& opp = player ? white : black;
		own |= turned;
		own |= bs64(1) << mi; // add played disk
		opp &= ~turned; 

		// it's now the other player's turn
		player = !player;
}

void Board::apply_move(const Move move) {
	bs64 discs = this->discs_to_turn(move);
	this->apply_move(discs, move.move);
}

//unoptimized but who cares?
void Board::apply_move(const int m) {
	if (m >= 0 && m <= 63) {
		std::vector<Move> moves = gen_move_list(gen_moves());
		auto it_m = std::find_if(moves.begin(), moves.end(), [=](const Move mo){ return mo.move == m; });
		assert(it_m != moves.end());
		apply_move(*it_m);
	} else {
		throw OutOfBoardMove(m);
	}
	
}

void Board::pass() {
	this->player = !(this->player);
}

bool Board::canPlay() const {
	std::array<bs64, 8> moves = this->gen_moves();
	for (const bs64 m : moves)
		if (m)
			return true;
	return false;
}

int Board::hasLegalMoves() const {
	bs64 own = player ? black : white;
	bs64 opp = player ? white : black;
	bs64 emp = ~(own | opp);

	// Check for a legal move for the player to play ;
	// return 1 if there is one.
	bs64 t = (own << 1) & RIGHT_MASK;
	while (t != 0) {
		t = ((t & opp) << 1) & RIGHT_MASK;
		if (t & emp) 
			return 1;
	}

	t = (own >> 1) & LEFT_MASK;
	while (t != 0) {
		t = ((t & opp) >> 1) & LEFT_MASK;
		if (t & emp) 
			return 1;	
	}

	t = (own >> 8) & UP_MASK;
	while (t != 0) {
		t = ((t & opp) >> 8) & UP_MASK;
		if (t & emp) 
			return 1;	
	}

	t = (own << 8) & DOWN_MASK;
	while (t != 0) {
		t = ((t & opp) << 8) & DOWN_MASK;
		if (t & emp) 
			return 1;	
	}

	t = (own >> 7) & UP_RIGHT_MASK;
	while (t != 0) {
		t = ((t & opp) >> 7) & UP_RIGHT_MASK;
		if (t & emp) 
			return 1;	
	}

	t = (own << 9) & DOWN_RIGHT_MASK;
	while (t != 0) {
		t = ((t & opp) << 9) & DOWN_RIGHT_MASK;
		if (t & emp) 
			return 1;	
	}

	t = (own << 7) & DOWN_LEFT_MASK;
	while (t != 0) {
		t = ((t & opp) << 7) & DOWN_LEFT_MASK;
		if (t & emp) 
			return 1;	
	}

	t = (own >> 9) & UP_LEFT_MASK;
	while (t != 0) {
		t = ((t & opp) >> 9) & UP_LEFT_MASK;
		if (t & emp) 
			return 1;	
	}

	//Now check for a legal move for the other player
	//if they have one, return 2
	own = (!player) ? black : white;
	opp = (!player) ? white : black;

	t = (own << 1) & RIGHT_MASK;
	while (t != 0) {
		t = ((t & opp) << 1) & RIGHT_MASK;
		if (t & emp) 
			return 2;
	}

	t = (own >> 1) & LEFT_MASK;
	while (t != 0) {
		t = ((t & opp) >> 1) & LEFT_MASK;
		if (t & emp) 
			return 2;	
	}

	t = (own >> 8) & UP_MASK;
	while (t != 0) {
		t = ((t & opp) >> 8) & UP_MASK;
		if (t & emp) 
			return 2;	
	}

	t = (own << 8) & DOWN_MASK;
	while (t != 0) {
		t = ((t & opp) << 8) & DOWN_MASK;
		if (t & emp) 
			return 2;	
	}

	t = (own >> 7) & UP_RIGHT_MASK;
	while (t != 0) {
		t = ((t & opp) >> 7) & UP_RIGHT_MASK;
		if (t & emp) 
			return 2;	
	}

	t = (own << 9) & DOWN_RIGHT_MASK;
	while (t != 0) {
		t = ((t & opp) << 9) & DOWN_RIGHT_MASK;
		if (t & emp) 
			return 2;	
	}

	t = (own << 7) & DOWN_LEFT_MASK;
	while (t != 0) {
		t = ((t & opp) << 7) & DOWN_LEFT_MASK;
		if (t & emp) 
			return 2;	
	}

	t = (own >> 9) & UP_LEFT_MASK;
	while (t != 0) {
		t = ((t & opp) >> 9) & UP_LEFT_MASK;
		if (t & emp) 
			return 2;	
	}

	//No one can play ; return 0
	return 0;
}


void Board::print() {
	bs64 current_bit_mask = bs64(1) << 63;
	for (unsigned i = 0; i < 64; ++i) {
		if      (black & current_bit_mask) std::cout << 'X';
		else if (white & current_bit_mask) std::cout << 'O';
		else 	  						   std::cout << '.';

		if (i % 8 == 7) NL;
		current_bit_mask >>= 1;
	}
}

/*
 * Converts the board into an array of 64 numbers :
 * 0 = empty
 * 1 = black
 * 2 = white
*/
std::array<uint8_t, 64> Board::toIntArray() const {
	std::array<uint8_t, 64> arr;
	for (unsigned i = 0; i < 64; ++i) {
		if ((white & BITn_MASK(i)) == (black & BITn_MASK(i)))
			arr[i] = 0;
		else {
			arr[i] = 1 + ((white & BITn_MASK(i)) > (black & BITn_MASK(i)));
		}
	}
	return arr;
}


int Board::getBorderDiscsScore() {
	bs64 empty = ~(black | white);
	int black_borders = (empty >> 1) & black & LEFT_MASK
					  | (empty << 1) & black & RIGHT_MASK
					  | (empty >> 8) & black
					  | (empty << 8) & black;
	int black_score = popcnt(black_borders);
	int white_borders = (empty >> 1) & white & LEFT_MASK
					  | (empty << 1) & white & RIGHT_MASK
					  | (empty >> 8) & white
					  | (empty << 8) & white;
	int white_score = popcnt(white_borders);
	return black_score - white_score;
}


/*
* . ==> empty
* 1 ==> black
* 0 ==> white
*/
Board str2board(const std::string& str, bool to_play) {
	assert(str.size() == 64);
	Board bb {0, 0, to_play};
	unsigned i = 63;
	for (const char c : str) {
		if (c == '1') 
			bb.black |= bs64(1) << i;
		else if (c == '0')
			bb.white |= bs64(1) << i;
		--i;
	}
	return bb;
}

Board move_seq2board(std::string seq) {
	//transform the move sequence to lower case
	std::transform(seq.begin(), seq.end(), seq.begin(), [](unsigned char c) {return tolower(c);});

	Board b {(bs64(1) << 35) | (bs64(1) << 28), // initial black discs
			  (bs64(1) << 36) | (bs64(1) << 27), // initial white discs
			  true}; // black to play
	for (unsigned i = 0; i < seq.size(); i += 2) {
		// pass if needed
		if (!b.canPlay()) {
			b.pass();
		}
    	//get move list
		std::vector<Move> ml = b.gen_move_list(b.gen_moves());

		//get move
		unsigned m = human_coords2int(seq.substr(i, 2));
		auto move = std::find_if(ml.begin(), ml.end(), [=](Move move){return move.move == m;});

		if (move == ml.end()) 
			std::cout << "Move not allowed." << std::endl;

		//play move
		b.apply_move(*move);
	}
	return b;
}