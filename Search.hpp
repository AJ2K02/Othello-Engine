#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "Board.hpp"

#include <functional>

struct Node {
	/*~Node() {
		if (ptr != nullptr) {
			~*ptr;
			delete ptr;
		}
	}*/

	Node* ptr = nullptr;
	_score_t score;
	int move = -1;
};

int eval();
int eval_ab();
int eval_position(Board& board);
int minimax(int n, bool max, int depth);
Node minimax_ab(Board& board, int n, bool max, int depth, _score_t alpha, _score_t beta, 
				std::function<_score_t(const Board&)> eval_func);

#endif