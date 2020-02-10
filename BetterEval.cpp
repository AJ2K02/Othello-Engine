#include "BetterEval.hpp"
#include "Utils.hpp"

#include <cmath>
#include <fstream>
#include <limits>

PatternValues::PatternValues() {
    m_edge.resize(std::numeric_limits<bs16>::max(), 0);
    m_edge_exp.resize(std::numeric_limits<bs16>::max(), 0);
    m_line.resize(std::numeric_limits<bs16>::max(), 0);
    m_line_exp.resize(std::numeric_limits<bs16>::max(), 0);
    // 2^20 possible states for the triangles
    m_triangle.resize(1 << 20, 0);
    m_triangle_exp.resize(1 << 20, 0);

    //resize everything and set the values to 0.
}

void PatternValues::learnGames(const std::vector<Game>& games, 
                               const unsigned fmove, const unsigned lmove) {
    Board board;
    for (const Game& game : games) {
        //int result = (game.scores[0] > 32) ? 1 : -1; 
        int result = game.scores[0] - game.scores[1];
        board = Board::StartingPosition();
        unsigned n_move { 0 };
        //Play the game :
        for (const MoveIndex move : game.moves) {
            if (n_move >= fmove && n_move <= lmove) {
                edgePatternUpdate(board, result);
                linePatternUpdate(board, result);
                trianglePatternUpdate(board, result);
            }
            //play next move
            try {
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
    }
}

void PatternValues::average() {
    // average edges
    for (unsigned i = 0; i < m_edge.size(); ++i) {
        if (m_edge_exp[i]  != 0)
            m_edge[i] /= m_edge_exp[i];
    }

    // average lines
    for (unsigned i = 0; i < m_line.size(); ++i) {
        if (m_line_exp[i]  != 0)
            m_line[i] /= m_line_exp[i];
    }

    // average triangles
    for (unsigned i = 0; i < m_triangle.size(); ++i) {
        if (m_triangle_exp[i]  != 0)
            m_triangle[i] /= m_triangle_exp[i];
    }
}


void PatternValues::useSymmetries() {
    edgeSymmetries();
    lineSymmetries();
    triangleSymmetries();
}

void PatternValues::edgeSymmetries() {
    // The following lambda reverses the 8 first bits of the number 
    // and separately reverses the 8 last bits
    // Ex (for batches of 3 bits instead of 8):
    // 0b001110 -> 0b100011
    auto reverse_key = [](const bs16 x) -> bs16 {
        bs16 y { 0 };
        for (unsigned i = 0; i < 8; ++i) {
            y |= (x & BITn_MASK(i)) >> i << (7 - i);
            y |= (x & BITn_MASK(i + 8)) >> i << (7 - i);
        }
        return y;
    };

    for (unsigned key = 0; key < m_edge.size(); ++key) {
        const unsigned symmetric_key = reverse_key(key);
        if (m_edge[key] + m_edge[symmetric_key]) {
            //We need to calculate the relative weight of each symmetry
            //for the average .
            const double k_weight = m_edge_exp[key] / (m_edge_exp[key] + m_edge_exp[symmetric_key]);
            const double sk_weight = m_edge_exp[symmetric_key] / (m_edge_exp[key] + m_edge_exp[symmetric_key]);
            const _score_t average = k_weight * m_edge[key] + sk_weight * m_edge[symmetric_key];
            
            //Update the score for the key and its symmetry
            m_edge[key] = m_edge[symmetric_key] = average;
        }
    }
}

void PatternValues::lineSymmetries() {
    // The following lambda reverses the 8 first bits of the number 
    // and separately reverses the 8 last bits
    // Ex (for batches of 3 bits instead of 8):
    // 0b001110 -> 0b100011
    auto reverse_key = [](const bs16 x) -> bs16 {
        bs16 y { 0 };
        for (unsigned i = 0; i < 8; ++i) {
            y |= (x & BITn_MASK(i)) >> i << (7 - i);
            y |= (x & BITn_MASK(i + 8)) >> i << (7 - i);
        }
        return y;
    };

    for (unsigned key = 0; key < m_line.size(); ++key) {
        const unsigned symmetric_key = reverse_key(key);
        if (symmetric_key >= m_line.size()) std::cout << "fuck " << key << ' ' << symmetric_key << std::endl;
        //We need to calculate the relative weight of each symmetry
        //for the average .
        if (m_line_exp[key] + m_line_exp[symmetric_key]) {
            const double k_weight = (double)m_line_exp[key] / (m_line_exp[key] + m_line_exp[symmetric_key]);
            const double sk_weight = (double)m_line_exp[symmetric_key] / (m_line_exp[key] + m_line_exp[symmetric_key]);
            const _score_t average = k_weight * m_line[key] + sk_weight * m_line[symmetric_key];
            
            //Update the score for the key and its symmetry
            m_line[key] = m_line[symmetric_key] = average;
        }
    }
}

void PatternValues::triangleSymmetries() {
    //90° rotations are already done because of the way triangles are encoded.
    //Only mirroring is needed.
    //Triangle : 
    // 6 7 8 9           6 3 1 0
    // 3 4 5     ---->   7 4 2
    // 1 2               8 5
    // 0                 9
    auto mirror_key = [](const bs32 x) -> bs32 {
        //keep bits 4 and 6 in place, swap others
        return (x & (BIT4_MASK | BIT6_MASK)) | (x & (BIT14_MASK | BIT16_MASK)) 
        | (x & BIT0_MASK) << 9 | (x & BIT10_MASK) << 9
        | (x & BIT1_MASK) << 7 | (x & BIT11_MASK) << 7
        | (x & BIT2_MASK) << 3 | (x & BIT12_MASK) << 3
        | (x & BIT3_MASK) << 4 | (x & BIT13_MASK) << 4
        | (x & BIT5_MASK) >> 3 | (x & BIT15_MASK) >> 3
        | (x & BIT7_MASK) >> 4 | (x & BIT17_MASK) >> 4
        | (x & BIT8_MASK) >> 7 | (x & BIT18_MASK) >> 7
        | (x & BIT9_MASK) >> 9 | (x & BIT19_MASK) >> 9;
    };

    for (unsigned key = 0; key < m_triangle.size(); ++key) {
        const unsigned mirrored = mirror_key(key);
        if (mirrored >= m_triangle.size()) std::cout << "fuck " << key << ' ' << mirrored << std::endl;
        //We need to calculate the relative weight of each symmetry
        //for the average .
        if (m_triangle_exp[key] + m_triangle_exp[mirrored]) {
            const unsigned long long d = m_triangle_exp[key] + m_triangle_exp[mirrored];
            const double k_weight = (double)m_triangle_exp[key] / d;
            const double mk_weight = (double)m_triangle_exp[mirrored] / d;
            const _score_t average = k_weight * m_triangle[key] + mk_weight * m_triangle[mirrored];
            
            //Update the score for the key and its symmetry
            m_triangle[key] = m_triangle[mirrored] = average;
        }
    }
}


void PatternValues::edgePatternUpdate(const Board& board, const int result) {
    std::vector<bs16> ind = edgePatternKeys(board);
    //update edge
    for (const bs16 edge : ind) {
        ++m_edge_exp[edge];
        m_edge[edge] += result;
    }
}

std::vector<bs16> PatternValues::edgePatternKeys(const Board& board) const {
    const bs16 black_edge_up = (board.black & EDGE_MASK_UP) >> 56;
    const bs16 white_edge_up = (board.white & EDGE_MASK_UP) >> 56;

    const bs16 black_edge_down = (board.black & EDGE_MASK_DOWN);
    const bs16 white_edge_down = (board.white & EDGE_MASK_DOWN);

    auto left_edge_lambda = [](bs64 x) -> bs16 {
        return (x & BIT63_MASK) >> 63 << 7
        | (x & BIT55_MASK) >> 55 << 6
        | (x & BIT47_MASK) >> 47 << 5
        | (x & BIT39_MASK) >> 39 << 4
        | (x & BIT31_MASK) >> 31 << 3
        | (x & BIT23_MASK) >> 23 << 2
        | (x & BIT15_MASK) >> 15 << 1
        | (x & BIT7_MASK) >> 7;
    };
    const bs16 black_edge_left = left_edge_lambda(board.black);
    const bs16 white_edge_left = left_edge_lambda(board.white);

    auto right_edge_lambda = [](bs64 x) -> bs16 {
        return (x & BIT56_MASK) >> 56 << 7
        | (x & BIT48_MASK) >> 48 << 6 
        | (x & BIT40_MASK) >> 40 << 5
        | (x & BIT32_MASK) >> 32 << 4
        | (x & BIT24_MASK) >> 24 << 3
        | (x & BIT16_MASK) >> 16 << 2
        | (x & BIT8_MASK) >> 8 << 1
        | (x & BIT0_MASK);
    };
    const bs16 black_edge_right = right_edge_lambda(board.black);
    const bs16 white_edge_right = right_edge_lambda(board.white);

    const bs16 edge_up = (black_edge_up << EDGE_NB_BITS) | white_edge_up;
    const bs16 edge_down = (black_edge_down << EDGE_NB_BITS) | white_edge_down;
    const bs16 edge_left = (black_edge_left << EDGE_NB_BITS) | white_edge_left;
    const bs16 edge_right = (black_edge_right << EDGE_NB_BITS) | white_edge_right;
    return { edge_up, edge_down, edge_left, edge_right };
}


void PatternValues::linePatternUpdate(const Board& board, const int result) {
    //bl : black discs on the line
    //wl : white discs on the line
    constexpr bs64 HLINE_MASK { 0xFF };
    constexpr bs64 VLINE_MASK { 0x0101010101010101 };
    constexpr unsigned LINE_LEN { 8 };
    
    //Horizontal lines first
    for (unsigned r = 2; r < 6; ++r) {
        bs16 bl = (board.black & (HLINE_MASK << (8 * r))) >> (8 * r);
        bs16 wl = (board.white & (HLINE_MASK << (8 * r))) >> (8 * r);
        bs16 line = (bl << LINE_LEN) | wl;
        ++m_line_exp[line];
        m_line[line] += result;
    }
    
    //Then vertical lines
    for (unsigned c = 2; c < 6; ++c) {
        bs16 bl = board.black & (VLINE_MASK << c);
        bs16 wl = board.white & (VLINE_MASK << c);
        bs16 line = (bl << LINE_LEN) | wl;
        ++m_line_exp[line];
        m_line[line] += result;
    }
}

std::vector<bs16> PatternValues::linePatternKeys(const Board& board) const {
    //bl : black discs on the line
    //wl : white discs on the line
    constexpr bs64 HLINE_MASK { 0xFF };
    constexpr bs64 VLINE_MASK { 0x0101010101010101 };
    constexpr unsigned LINE_LEN { 8 };

    std::vector<bs16> keys(8); //8 lines    
    //Horizontal lines first
    for (unsigned r = 2; r < 6; ++r) {
        bs16 bl = (board.black & (HLINE_MASK << (8 * r))) >> (8 * r);
        bs16 wl = (board.white & (HLINE_MASK << (8 * r))) >> (8 * r);
        keys[r-2] = (bl << LINE_LEN) | wl;
    }
    
    //Then vertical lines
    for (unsigned c = 2; c < 6; ++c) {
        bs16 bl = board.black & (VLINE_MASK << c);
        bs16 wl = board.white & (VLINE_MASK << c);
        keys[2+c] = (bl << LINE_LEN) | wl;
    }
    return keys;
}



void PatternValues::trianglePatternUpdate(const Board& board, const int result) {
    std::vector<bs32> triangles = trianglePatternKeys(board);
    for (const bs32 tr : triangles) {
        ++m_triangle_exp[tr];
        m_triangle[tr] += result;
    }
}

std::vector<bs32> PatternValues::trianglePatternKeys(const Board& board) const {
    constexpr unsigned TRIANGLE_LEN { 10 };

    auto top_left_lambda = [](const bs64 x) -> bs32 {
        return (x & BIT63_MASK) >> 63 << 6
        | (x & BIT62_MASK) >> 62 << 7
        | (x & BIT61_MASK) >> 61 << 8
        | (x & BIT60_MASK) >> 60 << 9
        | (x & BIT55_MASK) >> 55 << 3
        | (x & BIT54_MASK) >> 54 << 4
        | (x & BIT53_MASK) >> 53 << 5
        | (x & BIT47_MASK) >> 47 << 1
        | (x & BIT46_MASK) >> 46 << 2
        | (x & BIT39_MASK) >> 39;
    };

    auto top_right_lambda = [](const bs64 x) -> bs32 {
        return (x & BIT59_MASK) >> 59 << 9
        | (x & BIT58_MASK) >> 58 << 8
        | (x & BIT57_MASK) >> 57 << 7
        | (x & BIT56_MASK) >> 56 << 6
        | (x & BIT50_MASK) >> 50 << 5
        | (x & BIT49_MASK) >> 49 << 4
        | (x & BIT48_MASK) >> 48 << 3
        | (x & BIT41_MASK) >> 41 << 2
        | (x & BIT40_MASK) >> 40 << 1
        | (x & BIT32_MASK) >> 32;
    };

    auto bottom_right_lambda = [](const bs64 x) -> bs32 {
        return (x & BIT24_MASK) >> 24
        | (x & BIT17_MASK) >> 17 << 2
        | (x & BIT16_MASK) >> 16 << 1
        | (x & BIT10_MASK) >> 10 << 5
        | (x & BIT9_MASK) >> 9 << 4
        | (x & BIT8_MASK) >> 8 << 3
        | (x & BIT3_MASK) >> 3 << 9
        | (x & BIT2_MASK) >> 2 << 8
        | (x & BIT1_MASK) >> 1 << 7
        | (x & BIT0_MASK) << 6;
    };

    auto bottom_left_lambda = [](const bs64 x) -> bs32 {
        return (x & BIT31_MASK) >> 31
        | (x & BIT23_MASK) >> 23 << 1
        | (x & BIT22_MASK) >> 22 << 2
        | (x & BIT15_MASK) >> 15 << 3
        | (x & BIT14_MASK) >> 14 << 4
        | (x & BIT13_MASK) >> 13 << 5
        | (x & BIT7_MASK) >> 7 << 6
        | (x & BIT6_MASK) >> 6 << 7
        | (x & BIT5_MASK) >> 5 << 8
        | (x & BIT4_MASK) >> 4 << 9;
    };

    const bs32 bt_tl = top_left_lambda(board.black);
    const bs32 wt_tl = top_left_lambda(board.white);
    const bs32 bt_tr = top_right_lambda(board.black);
    const bs32 wt_tr = top_right_lambda(board.white);
    const bs32 bt_br = bottom_right_lambda(board.black);
    const bs32 wt_br = bottom_right_lambda(board.white);
    const bs32 bt_bl = bottom_left_lambda(board.black);
    const bs32 wt_bl = bottom_left_lambda(board.white);

    const bs32 tl = (bt_tl << TRIANGLE_LEN) | wt_tl;
    const bs32 tr = (bt_tr << TRIANGLE_LEN) | wt_tr;
    const bs32 br = (bt_br << TRIANGLE_LEN) | wt_br;
    const bs32 bl = (bt_bl << TRIANGLE_LEN) | wt_bl;
    return { tl, tr, br, bl };
}


void PatternValues::save(const std::string& path) {
    std::ofstream file;
    file.open(path, std::ios::binary | std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        file.write(reinterpret_cast<char*>(m_edge.data()), m_edge.size() * sizeof(_score_t));
        file.write(reinterpret_cast<char*>(m_line.data()), m_line.size() * sizeof(_score_t));
        file.write(reinterpret_cast<char*>(m_triangle.data()), m_triangle.size() * sizeof(_score_t));
        file.close();
        DEBUG_MSG("Values saved.");
    } else {
        std::cerr << "Couldn't open file in method PatternValues::save." << std::endl;
    }
}

void PatternValues::load(const std::string& path) {
    std::ifstream file;
    file.open(path, std::ios::in | std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(m_edge.data()), m_edge.size() * sizeof(_score_t));
        file.read(reinterpret_cast<char*>(m_line.data()), m_line.size() * sizeof(_score_t));
        file.read(reinterpret_cast<char*>(m_triangle.data()), m_triangle.size() * sizeof(_score_t));
        file.close();
        DEBUG_MSG("Values loaded");
    } else {
        std::cerr << "Couldn't open file in method PatternValues::load." << std::endl;
    }
}


/*
 * This evaluation function looks at different patterns
 * of the board and assigns a score to each of them.
 * The scores are then summed to get the score of the position.
 */ 
_score_t betterEval(const PatternValues& pv, const Board& board) {
    if (board.hasLegalMoves()) {
        // if the game is not finished
        _score_t score { 0 };
        // edges score
        const std::vector<bs16> edges = pv.edgePatternKeys(board);
        for (const bs16 edge : edges) {
            score += pv.m_edge[edge];
            //DEBUG_MSG(pv.m_edge[edge]);
        }

        // lines score
        const std::vector<bs16> lines = pv.linePatternKeys(board);
        for (const bs16 line : lines) {
            score += pv.m_line[line];
            //DEBUG_MSG(pv.m_line[line]);
        }
        // triangles score
        const std::vector<bs32> triangles = pv.trianglePatternKeys(board);
        for (const bs32 triangle : triangles) {
            score += pv.m_triangle[triangle];
            //DEBUG_MSG(pv.m_triangle[triangle]);
        }
        return score;
    } else {
        // if the game is finished, return the disc difference 
        // NOTE : not necessarily on the same scale as the other 
        // way to eval... should be fixed with weights for each pattern
        if (!~(board.black | board.white))
            return popcnt(board.black) - popcnt(board.white);
        else {
            //if there are empty squares
            const unsigned short black_discs = popcnt(board.black);
            const unsigned short white_discs = popcnt(board.white);
            if (black_discs > white_discs)
                //if black won
                return 64 - 2 * white_discs;
            else if (black_discs < white_discs)
                //if white won
                return 2 * black_discs - 64;
            else
                //draw 
                return 0;
        }
    }
}