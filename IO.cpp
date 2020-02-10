
#include "Eval.hpp"
#include "IO.hpp"
#include "Search.hpp"
#include "Utils.hpp"

#include <functional>

Interpreter::Interpreter() {
    m_board = Board::StartingPosition();
    m_eval_func = &basic_eval;
}

Interpreter::Interpreter(Board& board) {
    m_board = board;
    this->setCmdList();
    m_eval_func = &basic_eval;
}

void Interpreter::setCmdList() {
    m_cmds = {
        "exit",
        "play",
        "start",
        "eval"
    };
}

void Interpreter::run() {
    m_board.print();
    std::string cmd;
    while (1) {
        std::getline(std::cin, cmd);
        if (cmd.substr(0, 4) == "exit")
            break;
        else if (cmd.substr(0, 4) == "play")
            this->play(cmd);
        else if (cmd.substr(0, 4) == "eval")
            this->eval();
        else if (cmd.substr(0, 4) == "mode")
            this->mode(cmd);
        else if (cmd.substr(0, 4) == "show")
            this->show();
        else if (cmd.substr(0, 5) == "start")
            this->start();
        else if (cmd.substr(0, 9) == "showdepth")
            this->showdepth();
        else if (cmd.substr(0, 8) == "setdepth")
            this->setdepth(cmd);
        else if (cmd.substr(0, 8) == "selfplay")
            this->selfplay();
        else
            std::cout << "Unknown command..." << std::endl;
        std::cout << "---------------------------------" << std::endl;
    }
}

Interpreter Interpreter::setEvalFunc(std::function<_score_t(const Board&)> eval) {
    m_eval_func = eval;
    return *this;
}


void Interpreter::play(const std::string& cmd) {
    if (cmd.size() < 7) {
		std::cout << "Invalid move." << std::endl;
        return;
    }
    std::string move_str = cmd.substr(5, 2);
    int m = human_coords2int(move_str);

    std::vector<Move> ml = m_board.gen_move_list(m_board.gen_moves());
    // check if move is valid
	auto move = std::find_if(ml.begin(), ml.end(), [=](Move move){return move.move == m;});
	if (move == ml.end()) 
		std::cout << "Invalid move." << std::endl;
    else {
        m_board.apply_move(*move);
    }
    m_board.print();

    std::cout << " *** Border score : " << m_board.getBorderDiscsScore() << '\n';
    // answer if needed
    if (m_mode == 2 && m_board.player) {
        Node best = minimax_ab(m_board, 0, true, m_depth, -999, 999, m_eval_func);
        if (best.move != PASS_MOVE_INDEX) {
            m_board.apply_move(best.move);
            std::cout << "##### Answer : " << int2human_coords(best.move) << " #####" << std::endl;
        } else 
            std::cout << "##### Answer : pass #####" << std::endl;
        m_board.print();
    } else if (m_mode == 3 && !m_board.player) {
        Node best = minimax_ab(m_board, 0, false, m_depth, -999, 999, m_eval_func);
        if (best.move != PASS_MOVE_INDEX) {
            m_board.apply_move(best.move);
            std::cout << "##### Answer : " << int2human_coords(best.move) << " #####" << std::endl;
        } else 
            std::cout << "##### Answer : pass #####" << std::endl;
        m_board.print();
    }
}

void Interpreter::eval() {
    bool maximizing = m_board.player;
    Node best = minimax_ab(m_board, 0, maximizing, m_depth, -999.0, 999.0, m_eval_func);
    Node current = best;
    std::cout << "Moves : ";
	while (1) {
		if (current.ptr == nullptr)
			break;
		else
			std::cout << int2human_coords(current.move) << ' ';
		current = *current.ptr;
	}
    std::cout << "\nScore : " << current.score << std::endl;
}

void Interpreter::start() {
    m_board = Board::StartingPosition();
}

void Interpreter::show() {
    m_board.print();
}


void Interpreter::showdepth() const {
    std::cout << m_depth << std::endl;
}


void Interpreter::setdepth(const std::string& cmd) {
    if (cmd.size() < 10 || cmd.size() > 11) {
        // if the line is too short or too long.
        std::cout << "Invalid depth." << std::endl;
        return;
    } else if (cmd.size() == 10) {
        // if there are 10 characters <=> depth is a one-digit number
        m_depth = std::stoi(cmd.substr(9, 1));
    } else {
        // else it is a two-digits number
        m_depth = std::stoi(cmd.substr(9, 2));
    }
}

void Interpreter::selfplay() {
    std::array<std::string, 60> game_moves;
    unsigned move_nb = popcnt(m_board.black | m_board.white)-3;
    while (1) {
        const Node best = minimax_ab(m_board, 0, m_board.player, m_depth, -9999, 9999, m_eval_func);
        m_board.apply_move(best.move);
        game_moves[move_nb - 1] = int2human_coords(best.move); 

        // print move and board
        std::cout << "##### Move : " << move_nb << '.'
                  << game_moves[move_nb - 1] << " #####" << std::endl;
        m_board.print();

        // pass if needed / check if game is finished
        if (!m_board.canPlay()) {
            m_board.pass();
            if (!m_board.canPlay())
                break;
        }
        ++move_nb;
    }
    // Announce result
    std::cout << "Game finished. Score : " 
              << popcnt(m_board.black) << " - " 
              << popcnt(m_board.white) << std::endl;
    // Print moves
    for (const auto& m : game_moves) {
        std::cout << m;
    }
    std::cout << std::endl;
}

void Interpreter::mode(const std::string& cmd) {
    std::string invalid_usage_msg = 
                     "Invalid mode.\nUsage : mode <mode>.\n<mode> is a number between 1 and 3 :\n"
                     "1 : the engine doesn't play\n"
                     "2 : the engine plays black\n"
                     "3 : the engine plays white\n";
    if (cmd.size() != 6) {
        std::cout << invalid_usage_msg << std::flush;
        return;
    } else if (cmd[5] - '0' < 1 || cmd[5] - '0' > 3) {
        std::cout << invalid_usage_msg << std::flush;
    } else {
        m_mode = cmd[5] - '0';
    }
}