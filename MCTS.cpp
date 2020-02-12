#include "Board.hpp"
#include "MCTS.hpp"

#include <cmath>

Graph::Graph(const Board& board) {
    nodes.reserve(1024);
    nodes.push_back(TreeNode{-1, 0, 0, -1.0, nullptr, {}, board});
}

TreeNode Graph::goToLeafNode() {
    TreeNode current_node = nodes[0];
    while(current_node.children.size() != 0) {
        std::size_t best_i;
        for (std::size_t i = 0; i < current_node.children.size(); ++i) {
            double best_score = -1;
            //if uct_score had never been calculated, do it now
            if (current_node.children[i]->uct_score == -1.0) {
                current_node.children[i]->calculateUCT();
            }
            //find the best child according to its uct_score
            if (current_node.children[i]->uct_score > best_score) {
                best_i = i;
                best_score = current_node.children[i]->uct_score;
            }
            current_node = *current_node.children[best_i];
        }
    }
    return current_node;
}

void TreeNode::calculateUCT() {
    uct_score = (double)wins / games + 
        UCT_CONSTANT * sqrt(std::log(parent->games) / games);    
}


TreeNode mctsSearch(const Board& board) {
    Board current_board = board;
    Graph tree(board);
    while(true) {
        TreeNode leaf = tree.goToLeafNode();
        current_board = leaf.board;
        //TODO : create new leaf, simulation, backprop
    }
}