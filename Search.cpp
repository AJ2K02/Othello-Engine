
#include "Search.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>

#include "Board.hpp"
#include "Config.hpp"
#include "Eval.hpp"
#include "Utils.hpp"

static int seed = std::chrono::steady_clock::now().time_since_epoch().count();

int eval() {
	//int seed = std::chrono::steady_clock::now().time_since_epoch().count();
	static thread_local std::mt19937 gen(seed);
	std::uniform_int_distribution<int> distr(-10, 10);
	int out = distr(gen);
	std::cout << out << " ";
	return out;
}

int eval_ab() {
	//int seed = std::chrono::steady_clock::now().time_since_epoch().count();
	static thread_local std::mt19937 gen(seed);
	std::uniform_int_distribution<int> distr(-10, 10);
	int out = distr(gen);
	std::cout << out << " ";
	return out;
}

int eval_position(Board& board) {
	return popcnt(board.black);
}

int minimax(int n, bool max, int depth) {
	if (n == depth)
		return eval();

	if (max) {
		int best_score = std::numeric_limits<int>::min(); 
		for (unsigned i = 0; i < 4; ++i) {
			int score = minimax(n+1, !max, depth);
			if (score > best_score) 
				best_score = score;
		}
		return best_score;
	} else {
		int best_score = std::numeric_limits<int>::max(); 
		for (unsigned i = 0; i < 4; ++i) {
			int score = minimax(n+1, !max, depth);
			if (score < best_score) 
				best_score = score;
		}
		return best_score;
	}
}

Node minimax_ab(Board& board, int n, bool max, int depth, _score_t alpha, _score_t beta,
				std::function<_score_t(const Board&)> eval_func) {
	if (n == depth) {
		return Node{nullptr, eval_func(board), -1};
	}

	// Generate a list of all legal moves
	std::vector<Move> move_list = board.gen_move_list(board.gen_moves());

	if (move_list.size() > 0) {
		if (max) {
			// maximizing player
			//NOTE : use of numeric_limits::lowest() instead of min() because
			//min() actually gives the smallest positive number for 
			//types float/double/long double
			Node best_move {nullptr, std::numeric_limits<_score_t>::lowest(), -2}; 
			best_move.ptr = new Node;
			Node current;
			for (Move m : move_list) {
				Board new_board = board;
				new_board.apply_move(m);
				current = minimax_ab(new_board, n+1, !max, depth, alpha, beta, eval_func);
				if (current.score > best_move.score) {
					// update best_move
					best_move.score = current.score;
					best_move.ptr->ptr = current.ptr;
					best_move.ptr->score = current.score; 
					best_move.ptr->move = current.move;
					best_move.move = m.move;
				}
				if (beta <= best_move.score) {
					return best_move;
				}
				if (alpha < best_move.score)
					alpha = best_move.score;
			}
			return best_move;
		} else {
			// minimizing player
			Node best_move {nullptr, std::numeric_limits<_score_t>::max(), -3}; 
			best_move.ptr = new Node;
			Node current;
			for (Move m : move_list) {
				Board new_board = board;
				new_board.apply_move(m);
				current = minimax_ab(new_board, n+1, !max, depth, alpha, beta, eval_func);
				if (current.score < best_move.score) {
					// update best_move
					best_move.score = current.score;
					best_move.ptr->ptr = current.ptr;
					best_move.ptr->score = current.score; 
					best_move.ptr->move = current.move;
					best_move.move = m.move;
				}
				if (best_move.score <= alpha) {
					return best_move;
				}
				if (best_move.score < beta)
					beta = best_move.score;
			}
			return best_move;
		}
	} else {
		// In case there's no legal move
		// Create new board and pass
		Board new_board = board;
		new_board.pass();
		// Create node and return
		Node current = minimax_ab(new_board, n+1, !max, depth, alpha, beta, eval_func);
		Node best_move {new Node, current.score, PASS_MOVE_INDEX};
		best_move.ptr->ptr = current.ptr;
		best_move.ptr->score = current.score;
		best_move.ptr->move = current.move;
		return best_move;
	}
}

/*
int minimax_ab(int n, bool max, int depth, int alpha, int beta) {
	if (n == depth) {
		return eval_ab();
	}

	if (max) {
		// maximizing player
		int best_score = std::numeric_limits<int>::min(); 
		for (unsigned i = 0; i < 4; ++i) {
			int score = minimax_ab(n+1, !max, depth, alpha, beta);
			if (score > best_score) {
				best_score = score; 
			}
			if (beta <= best_score) {
				return best_score;
			}
			if (alpha < best_score)
				alpha = best_score;
		}
		return best_score;
	} else {
		// minimizing player
		int best_score = std::numeric_limits<int>::max(); 
		for (unsigned i = 0; i < 4; ++i) {
			int score = minimax_ab(n+1, !max, depth, alpha, beta);
			if (score < best_score) 
				best_score = score;
			if (best_score <= alpha) {
				return best_score;
			}
			if (best_score < beta)
				beta = best_score;
		}
		return best_score;
	}
}
*/
