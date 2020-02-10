#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include "Board.hpp"
#include "Eval.hpp"
#include "Utils.hpp"


CESTL std::array<std::array<unsigned, 10>, 8> EDGE_X_DISCS_WITH_SYMMETRIES {
    std::array<unsigned, 10>{ 63, 62, 61, 60, 59, 58, 57, 56, 54, 49 },
    std::array<unsigned, 10>{ 0, 1, 2, 3, 4, 5, 6, 7, 9, 14 },
    std::array<unsigned, 10>{ 56, 48, 40, 32, 24, 16, 8, 0, 49, 9 },
    std::array<unsigned, 10>{ 7, 15, 23, 31, 39, 47, 55, 63, 14, 54 },
    std::array<unsigned, 10>{ 56, 57, 58, 59, 60, 61, 62, 63, 49, 54 },
    std::array<unsigned, 10>{ 7, 6, 5, 4, 3, 2, 1, 0, 14, 9 },
    std::array<unsigned, 10>{ 0, 8, 16, 24, 32, 40, 48, 56, 9, 49 },
    std::array<unsigned, 10>{ 63, 55, 47, 39, 31, 23, 15, 7, 54, 14 }
};

CESTL std::array<std::array<unsigned, 10>, 4> EDGE_X_DISCS {
    std::array<unsigned, 10>{ 63, 62, 61, 60, 59, 58, 57, 56, 54, 49 },
    std::array<unsigned, 10>{ 0, 1, 2, 3, 4, 5, 6, 7, 9, 14 },
    std::array<unsigned, 10>{ 56, 48, 40, 32, 24, 16, 8, 0, 49, 9 },
    std::array<unsigned, 10>{ 7, 15, 23, 31, 39, 47, 55, 63, 14, 54 }
}; 

CESTL std::array<std::array<unsigned, 9>, 8> CORNER33_DISCS_WITH_SYMMETRIES {
    std::array<unsigned, 9> { 0, 1, 2, 8, 9, 10, 16, 17, 18 },
    std::array<unsigned, 9> { 7, 15, 23, 6, 14, 22, 5, 13, 21 },
    std::array<unsigned, 9> { 63, 62, 61, 55, 54, 53, 47, 46, 45 },
    std::array<unsigned, 9> { 56, 48, 40, 57, 49, 41, 58, 50, 42 },
    std::array<unsigned, 9> { 0, 8, 16, 1, 9, 17, 2, 10, 18 },
    std::array<unsigned, 9> { 7, 6, 5, 15, 14, 13, 23, 22, 21 },
    std::array<unsigned, 9> { 63, 55, 47, 62, 54, 46, 61, 53, 45 },
    std::array<unsigned, 9> { 56, 57, 58, 48, 49, 50, 40, 41, 42 }
};

CESTL std::array<std::array<unsigned, 9>, 4> CORNER33_DISCS {
    std::array<unsigned, 9> { 0, 1, 2, 8, 9, 10, 16, 17, 18 },
    std::array<unsigned, 9> { 7, 15, 23, 6, 14, 22, 5, 13, 21 },
    std::array<unsigned, 9> { 63, 62, 61, 55, 54, 53, 47, 46, 45 },
    std::array<unsigned, 9> { 56, 48, 40, 57, 49, 41, 58, 50, 42 }
};

CESTL std::array<std::array<unsigned, 10>, 8> CORNER25_DISCS_WITH_SYMMETRIES {
    std::array<unsigned, 10> { 0, 1, 2, 3, 4, 8, 9, 10, 11, 12 },
    std::array<unsigned, 10> { 7, 15, 23, 31, 39, 6, 14, 22, 30, 38 },
    std::array<unsigned, 10> { 63, 62, 61, 60, 59, 55, 54, 53, 52, 51 },
    std::array<unsigned, 10> { 56, 48, 40, 32, 24, 57, 59, 51, 33, 25 },
    std::array<unsigned, 10> { 0, 8, 16, 24, 32, 1, 9, 17, 25, 33 },
    std::array<unsigned, 10> { 7, 6, 5, 4, 3, 15, 14, 13, 12, 11 },
    std::array<unsigned, 10> { 63, 55, 47, 39, 31, 62, 54, 46, 38, 30 },
    std::array<unsigned, 10> { 56, 57, 58, 59, 60, 48, 49, 50, 51, 52 }
};

CESTL std::array<std::array<unsigned, 10>, 4> CORNER25_DISCS {
    std::array<unsigned, 10> { 0, 1, 2, 3, 4, 8, 9, 10, 11, 12 },
    std::array<unsigned, 10> { 7, 15, 23, 31, 39, 6, 14, 22, 30, 38 },
    std::array<unsigned, 10> { 63, 62, 61, 60, 59, 55, 54, 53, 52, 51 },
    std::array<unsigned, 10> { 56, 48, 40, 32, 24, 57, 59, 51, 33, 25 }
};

CESTL std::array<std::array<unsigned, 8>, 4> DIAG8_DISCS_WITH_SYMMETRIES {
    std::array<unsigned, 8> { 0, 9, 18, 27, 36, 45, 54, 63 },
    std::array<unsigned, 8> { 7, 14, 21, 28, 35, 42, 49, 56 },
    std::array<unsigned, 8> { 63, 54, 45, 36, 27, 18, 9, 0 },
    std::array<unsigned, 8> { 56, 49, 42, 35, 28, 21, 14, 7 }
};

CESTL std::array<std::array<unsigned, 8>, 2> DIAG8_DISCS {
    std::array<unsigned, 8> { 0, 9, 18, 27, 36, 45, 54, 63 },
    std::array<unsigned, 8> { 7, 14, 21, 28, 35, 42, 49, 56 }
};

CESTL auto ALL_PATTERNS = std::make_tuple(EDGE_X_DISCS, CORNER33_DISCS, 
                                          CORNER25_DISCS, DIAG8_DISCS);

CESTL auto ALL_PATTERNS_WITH_SYMMETRIES = std::make_tuple(EDGE_X_DISCS_WITH_SYMMETRIES,
                                                          CORNER33_DISCS_WITH_SYMMETRIES,
                                                          CORNER25_DISCS_WITH_SYMMETRIES,
                                                          DIAG8_DISCS_WITH_SYMMETRIES);

CESTL std::size_t N_PATTERNS = std::tuple_size<decltype(ALL_PATTERNS)>::value;

template<unsigned N>
constexpr std::array<unsigned, N> createWeights() {
    std::array<unsigned, N> w {};
    for (int i = 0; i < w.size(); ++i) {
        w[i] = std::pow(3, i);
    }
    return w;
} 


/*
 *  Process to add a new pattern:
 *  add the 3 corresponding members
 *  resize vectors in the constructor
 *  getXXXPatterns()
 *  XXXUpdate()
 *  getXXX()
 *  change learnGame()
 *  change average()
 *  change evalPosition()
 *  change save()
 *  change load()
 */ 
class Evaluator {
    public:
    using _weight_t = double;
    Evaluator();

    void learnGames(const std::vector<Game>& games, const unsigned fmove=0, const unsigned lmove=60);
    void averageStats();
    void learnWeights(const std::vector<Game>& games);
    //std::array<_score_t, N_PATTERNS> getFeatureScores(const Board& board);

    template<std::size_t ...I, typename ...Args>
    std::array<std::pair<_score_t, _score_t>, sizeof...(I)> getScores(const std::array<uint8_t, 64>& b, std::index_sequence<I...>, Args&& ...args);
    _score_t evalPosition(const Board& board);

    std::vector<std::vector<_score_t>> getPatterns() const { return m_patterns; }
    std::vector<std::vector<_score_t>> getPatternsVar() const { return m_patterns_var; }
    std::vector<std::vector<unsigned long>> getPatternsExp() const { return m_patterns_exp; }
    std::array<_weight_t, N_PATTERNS> getWeights() const { return m_patterns_w; }
    _weight_t getBias() const { return m_bias; }

    template<unsigned NPatterns, unsigned NDiscs>
    std::array<unsigned, NPatterns> getPatterns(const std::array<uint8_t, 64>& board,
                        const std::array<std::array<unsigned, NDiscs>, NPatterns>& discs);

    template<unsigned NPatterns, unsigned NDiscs>
    void updatePattern(const std::array<uint8_t, 64>& board,
                       const int result,
                       const unsigned pattern_n,
                       const std::array<std::array<unsigned, NDiscs>, NPatterns>& discs);

    //Store parameters in a file
    void save(const std::string& file);
    void load(const std::string& file);
    void saveFeatures(const std::string& path, 
                      std::vector<std::array<_score_t, N_PATTERNS>>& features, 
                      std::vector<int> results,
                      std::vector<int> empties);

    //////////////////////
    // ---VARIABLES---  //
    //////////////////////

    private:

    std::array<_weight_t, N_PATTERNS> m_patterns_w;
    _weight_t m_bias { 0 };
    std::vector<std::vector<_score_t>> m_patterns;
    std::vector<std::vector<_score_t>> m_patterns_var;
    std::vector<std::vector<unsigned long>> m_patterns_exp;
};


template<unsigned NPatterns, unsigned NDiscs>
std::array<unsigned, NPatterns> Evaluator::getPatterns(const std::array<uint8_t, 64>& board,
                        const std::array<std::array<unsigned, NDiscs>, NPatterns>& discs) {
    CESTL std::array<unsigned, NDiscs> w = createWeights<NDiscs>();
    std::array<unsigned, NPatterns> ind;
    std::fill(ind.begin(), ind.end(), 0);
    for (int i = 0; i < NPatterns; ++i) {
        for (int j = 0; j < NDiscs; ++j) {
            ind[i] += w[j] * board[discs[i][j]];
        }
    }
    return ind;
}


template<unsigned NPatterns, unsigned NDiscs>
void Evaluator::updatePattern(const std::array<uint8_t, 64>& board,
                              const int result, 
                              const unsigned pattern_n,
                              const std::array<std::array<unsigned, NDiscs>, NPatterns>& discs) {
    auto ind = getPatterns(board, discs);
    for (unsigned long i : ind) {
        m_patterns[pattern_n][i] += result;
        m_patterns_var[pattern_n][i] += SQR(result);
        ++m_patterns_exp[pattern_n][i];
    }
}


/*
 * ...args contains all the arrays of arrays of discs (one per pattern type)
 */ 
template<std::size_t ...I, typename ...Args>
std::array<std::pair<_score_t, _score_t>, sizeof...(I)> Evaluator::getScores(const std::array<uint8_t, 64>& b, std::index_sequence<I...>, Args&& ...args) {
    const auto get_temp_score = [&](auto&& arr, const std::size_t pn) -> std::pair<_score_t, _score_t> {
            //arr is an array of arrays to give to getPatterns
            auto patt = getPatterns(b, arr);
            // patt : array of unsigned long
            const auto l = [pn,this](_score_t s, unsigned long x){ return s + m_patterns[pn][x]; };
            const auto l_var = [pn, this](_score_t s, unsigned long x){ return s + m_patterns_var[pn][x]; };
            return std::make_pair<_score_t, _score_t>(std::accumulate(patt.begin(), patt.end(), _score_t{ 0 }, l),
                                                      std::accumulate(patt.begin(), patt.end(), _score_t{ 0 }, l_var)); 
    };
    return std::array<std::pair<_score_t, _score_t>, sizeof...(I)>{ get_temp_score(args, I)... };
}