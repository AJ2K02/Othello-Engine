#include "PatternEval.hpp"
#include "Utils.hpp"
#include <cmath>
#include <fstream>
#include <numeric>
#include <string>
#include <tuple>
#include <utility>


Evaluator::Evaluator() {
    m_patterns.resize(N_PATTERNS);
    m_patterns_var.resize(N_PATTERNS);
    m_patterns_exp.resize(N_PATTERNS);

    m_patterns[0].resize(std::pow(3, 10), 0);
    m_patterns_var[0].resize(std::pow(3, 10), 0);
    m_patterns_exp[0].resize(std::pow(3, 10), 0);
    
    m_patterns[1].resize(std::pow(3, 9), 0);
    m_patterns_var[1].resize(std::pow(3, 9), 0);
    m_patterns_exp[1].resize(std::pow(3, 9), 0);
    
    m_patterns[2].resize(std::pow(3, 10), 0);
    m_patterns_var[2].resize(std::pow(3, 10), 0);
    m_patterns_exp[2].resize(std::pow(3, 10), 0);
    
    m_patterns[3].resize(std::pow(3, 8), 0);
    m_patterns_var[3].resize(std::pow(3, 8), 0);
    m_patterns_exp[3].resize(std::pow(3, 8), 0);

    std::fill(m_patterns_w.begin(), m_patterns_w.end(), 1);
}


void Evaluator::learnGames(const std::vector<Game>& games, const unsigned fmove, const unsigned lmove) {
    Board board;
    unsigned ng {0};
    for (const Game& game : games) {
        //int result = (game.scores[0] > 32) ? 1 : -1; 
        int result =  2 * (game.scores[0] - 32);
        board = Board::StartingPosition();
        unsigned n_move { 0 };
        //Play the game :
        for (const MoveIndex move : game.moves) {
            if (n_move >= fmove && n_move <= lmove) {
                std::array<uint8_t, 64> b = board.toIntArray();
                updatePattern(b, result, 0, EDGE_X_DISCS_WITH_SYMMETRIES);
                updatePattern(b, result, 1, CORNER33_DISCS_WITH_SYMMETRIES);
                updatePattern(b, result, 2, CORNER25_DISCS_WITH_SYMMETRIES);
                updatePattern(b, result, 3, DIAG8_DISCS_WITH_SYMMETRIES);
            }
            //play next move
            try {
                //std::cout << int2human_coords(fromWthorCoords(move));
                board.apply_move(fromWthorCoords(move));
            } catch(const OutOfBoardMove& exc) {
                DEBUG_MSG("OutOfBoardMove thrown");
                break;
            }
            ++n_move;
            //check if a pass is needed and if yes if the game is finished
            if (!board.canPlay()) {
                board.pass();
                if (!board.canPlay()) 
                    // break if the game is finished
                    break;
            }
        }
        /*++ng;
        if (ng > 1) break;*/
    }
    averageStats();
}


void Evaluator::averageStats() {
    for (int p = 0; p < m_patterns.size(); ++p)
        for (int i = 0; i < m_patterns[p].size(); ++i) {
            if (m_patterns_exp[p][i]) { 
                m_patterns[p][i] /= m_patterns_exp[p][i];
                m_patterns_var[p][i] /= m_patterns_exp[p][i];
                m_patterns_var[p][i] -= SQR(m_patterns[p][i]);
                m_patterns_var[p][i] = std::sqrt(m_patterns_var[p][i]);
            }
        }
}


void Evaluator::learnWeights(const std::vector<Game>& games) {
    constexpr unsigned EPOCHS { 1 };
    constexpr double ETA { 0.00001 };
    unsigned it = 0;
    Board board;
    std::vector<std::array<_score_t, N_PATTERNS>> features_vec;
    std::vector<int> results_vec;
    std::vector<int> empties_vec;

    features_vec.reserve(games.size() * 60);
    results_vec.reserve(games.size() * 60);
    empties_vec.reserve(games.size() * 60);

    for (unsigned epoch = 0; epoch < EPOCHS; ++epoch) {
        // Iterate on all games
        for (const Game& game : games) {
            board = Board::StartingPosition();
            int result =  2 * (game.scores[0] - 32);
            // Iterate on all moves
            int n_move { 0 }; 
            for (uint8_t move : game.moves) {
                if (!board.canPlay()) {
                    board.pass();
                    if (!board.canPlay()) 
                        // break if the game is finished
                        break;
                }

                //play next move
                try {
                    board.apply_move(fromWthorCoords(move));
                    ++n_move;
                } catch(const OutOfBoardMove& exc) {
                    DEBUG_MSG("OutOfBoardMove thrown");
                    break;
                }

                //std::array<_score_t, N_PATTERNS> features = getFeatureScores(board);
                std::array<_score_t, N_PATTERNS> features;
                features_vec.push_back(features);
                results_vec.push_back(result);
                empties_vec.push_back(n_move);
                /*_score_t y = std::inner_product(features.begin(), features.end(), m_patterns_w.begin(), m_bias);
                _score_t loss = (y - result) * (y - result); // * 0.5
                _score_t dldy = y - result;
                for (std::size_t i = 0; i < m_patterns_w.size(); ++i) {
                    m_patterns_w[i] -= ETA * dldy * features[i];
                }
                m_bias -= ETA * dldy;*/
                ++it;
                if (it % 10000 == 0) DEBUG_MSG("yaaay");
                if (it == 1 || it == 2) {
                    printCtn(features); std::cout << ' ' << result << std::endl;
                }
            }
        }
    }
    saveFeatures("data/Features.bin", features_vec, results_vec, empties_vec);
}

/*
std::array<_score_t, N_PATTERNS> Evaluator::getFeatureScores(const Board& board) {
    std::array<uint8_t, 64> b = board.toIntArray();
        
    CESTL auto idx_seq = std::make_index_sequence<N_PATTERNS>();
    _score_t score { 0 };

    const auto get_scores_wrapper = [&](auto&& ...args){ return getScores(b, idx_seq, args...); };
    return std::apply(get_scores_wrapper, ALL_PATTERNS);
}*/


_score_t Evaluator::evalPosition(const Board& board) {
    if (board.hasLegalMoves()) {
        std::array<uint8_t, 64> b = board.toIntArray();
        
        CESTL auto idx_seq = std::make_index_sequence<N_PATTERNS>();
        _score_t score { 0 };

        const auto get_scores_wrapper = [&](auto&& ...args){ return getScores(b, idx_seq, args...); };
        auto scores = std::apply(get_scores_wrapper, ALL_PATTERNS);

        //eval = sum weighted by stddev of each feature pattern
        _score_t eval { 0 };
        _score_t total_weights { 0 };
        for (std::size_t i = 0; i < scores.size(); ++i) {
            if (scores[i].second != 0) {
                eval += scores[i].first / scores[i].second;
                total_weights += 1.0 / scores[i].second;
            }
        }
        if (total_weights != 0) return eval / total_weights;
        else return 0;
        //return std::accumulate(scores.begin(), scores.end(), 1);
        //return std::inner_product(scores.begin(), scores.end(), m_patterns_w.begin(), m_bias);
    } else {
        return popcnt(board.black) - popcnt(board.white);
    }
}


void Evaluator::save(const std::string& path) {
    std::ofstream file;
    file.open(path, std::ios::binary | std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        for (std::size_t i = 0; i < m_patterns.size(); ++i) {
            file.write(reinterpret_cast<char*>(m_patterns[i].data()), m_patterns[i].size() * sizeof(_score_t));
            file.write(reinterpret_cast<char*>(m_patterns_var[i].data()), m_patterns_var[i].size()*sizeof(_score_t));       
        }
        file.write(reinterpret_cast<char*>(m_patterns_w.data()), sizeof(_weight_t) * N_PATTERNS);
        file.write(reinterpret_cast<char*>(&m_bias), sizeof(_weight_t));
        file.close();
        DEBUG_MSG("Values saved.");
    } else {
        std::cerr << "Couldn't open file in method Evaluator::save." << std::endl;
    }
}


void Evaluator::load(const std::string& path) {
    std::ifstream file;
    file.open(path, std::ios::in | std::ios::binary);
    if (file.is_open()) {
        for (std::size_t i = 0; i < m_patterns.size(); ++i) {
            file.read(reinterpret_cast<char*>(m_patterns[i].data()), m_patterns[i].size() * sizeof(_score_t));
            file.read(reinterpret_cast<char*>(m_patterns_var[i].data()), m_patterns_var[i].size()*sizeof(_score_t));       
        }
        file.read(reinterpret_cast<char*>(m_patterns_w.data()), sizeof(_weight_t) * N_PATTERNS);
        file.read(reinterpret_cast<char*>(&m_bias), sizeof(_weight_t));
        file.close();
        DEBUG_MSG("Values loaded");
    } else {
        std::cerr << "Couldn't open file in method Evaluator::load." << std::endl;
    }
}


void Evaluator::saveFeatures(const std::string& path, 
                             std::vector<std::array<_score_t, N_PATTERNS>>& features, 
                             std::vector<int> results,
                             std::vector<int> empties) {
    std::ofstream file;
    file.open(path, std::ios::trunc | std::ios::out | std::ios::binary);
    if (file.is_open()) {
        assert(features.size() == results.size());
        int sz = features.size();
        std::cout << "Size of int : " << sizeof(int) << std::endl;
        int sz_of_real = sizeof(_score_t);
        file.write(reinterpret_cast<char*>(&sz), sizeof(int));
        file.write(reinterpret_cast<char*>(&sz_of_real), sizeof(int));
        
        for (std::size_t i = 0; i < features.size(); ++i) {
            file.write(reinterpret_cast<char*>(&(features[i])), sizeof(_score_t) * N_PATTERNS);
            file.write(reinterpret_cast<char*>(&(results[i])), sizeof(int));
            file.write(reinterpret_cast<char*>(&(empties[i])), sizeof(int));
        }
        file.close();
    } else {
        std::cerr << "Couldn't open file in method Evaluator::saveFeatures." << std::endl;
    }
}
