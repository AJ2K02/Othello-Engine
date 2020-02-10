#pragma once

#include "Eval.hpp"

#define EDGE_MASK_UP    0xFF00000000000000
#define EDGE_MASK_DOWN  0x00000000000000FF
#define EDGE_NB_BITS 8


/*
 * This structure contains all the weights of every possible 
 * states of the different patterns that are considered when
 * judging a position. 
 * 
 * The values for a pattern are stored in a std::vector, and
 * the key to a position is the number :
 * (black << n) | white
 * where n is the number of squares in the pattern
 * 
 * The implemented patterns are the following :
 *  - edge (8 squares)
 * 
 * For each pattern there are two member variables : 
 * m_pattern and m_pattern_exp
 * The exp version contains the number of examples it has seen
 * for each possible pattern
 */
struct PatternValues {
    PatternValues();

    void learnGames(const std::vector<Game>& games, const unsigned fmove=0, const unsigned lmove=60);
    void average();
    void useSymmetries();
    void edgeSymmetries();
    void lineSymmetries();
    void triangleSymmetries();


    void edgePatternUpdate(const Board& board, const int result);
    std::vector<bs16> edgePatternKeys(const Board& board) const;
    void linePatternUpdate(const Board& board, const int result);
    std::vector<bs16> linePatternKeys(const Board& board) const;
    void trianglePatternUpdate(const Board& board, const int result);
    std::vector<bs32> trianglePatternKeys(const Board& board) const;

    void save(const std::string& path);
    void load(const std::string& path);

    //////////////////////
    // ---VARIABLES---  //
    //////////////////////

    std::vector<_score_t> m_edge;
    std::vector<unsigned long> m_edge_exp;
    std::vector<_score_t> m_line;
    std::vector<unsigned long> m_line_exp;
    std::vector<_score_t> m_triangle;
    std::vector<unsigned long> m_triangle_exp;

};

_score_t betterEval(const PatternValues& pv, const Board& board);