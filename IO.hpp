#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "Board.hpp"

/*
 * Class managing the input/output to the console.
 * 
 * Commands : 
 * exit
 * play <f5>
 * eval
 * mode <1 | 2 | 3> : inactive, engine black, engine white
 * show 
 * start
 * setdepth <n>
 * showdepth
 * selfplay
 * 
 */ 
class Interpreter {
    public:
    Interpreter();
    Interpreter(Board& board);

    void run();
    Interpreter setEvalFunc(std::function<_score_t(const Board&)> eval);

    // Functions called when their corresponding command is written
    void play(const std::string& cmd);
    void eval();
    void start();
    void show();
    void showdepth() const;
    void setdepth(const std::string& cmd);
    void selfplay();
    void mode(const std::string& cmd);

    private:
    Board m_board;
    std::vector<std::string> m_cmds;
    int m_depth { 8 };
    int m_mode { 1 };
    std::function<_score_t(const Board&)> m_eval_func;

    void setCmdList();
};