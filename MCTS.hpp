#pragma once
#include "Board.hpp"

#include <vector>

#define UCT_CONSTANT 1.4

/*
create a Graph (initially with one Node, the original position)
while (search still has time) :
    MCTS steps :
    selection : use UCT to go to a leaf node of the current graph
    expansion : choose a child move (randomly or alphabeta) and add it to the graph
    simulation : simulate a game from this position
    update each node between the root and the newly created node with the result
      of the simulation. (update wins, games, uct_score)
*/


struct TreeNode;

struct Graph {
    std::vector<TreeNode> nodes;

    Graph(const Board& board);
    TreeNode goToLeafNode();
};

struct TreeNode {
    uint8_t move;
    unsigned wins;
    unsigned games;
    double uct_score;
    TreeNode* parent;
    std::vector<TreeNode*> children;
    Board board;

    void calculateUCT();
};

TreeNode mctsSearch(const Board& board);