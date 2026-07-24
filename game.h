#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <random>
#include <limits>
#include "card.h"
#include "engine.h"

class Game {
private:
    static constexpr int PREFLOP = 0;
    static constexpr int FLOP = 1;
    static constexpr int TURN = 2;
    static constexpr int RIVER = 3;
    static constexpr int SHOWDOWN = 4;

    static constexpr int CHECK = 1;
    static constexpr int RAISE = 2;
    static constexpr int CALL = 3;
    static constexpr int FOLD = 4;

    std::random_device rd;
    std::mt19937 gen;

    Hand A;
    Hand B;

    int aStack;
    int bStack;

    int stage;
    int pot;
    int dealer; // 0 for A, 1 for B
    int communityDealt;

    uint8_t communityCards[5];
    uint8_t holeA[2];
    uint8_t holeB[2];   

    int dealt;
    uint8_t deck[52];

    int getAction(int opposingBet, int curBet, bool invalid);
    int processAction(int action, int &aBet, int &bBet, bool &lastCheck, bool aTurn);
    void handleBet(int &aBet, int &bBet);
    void clearTerminal();
    std::string getStageString();
public:
    const int smallBlind = 10;
    const int bigBlind = 20;

    Game();
    void dealHand();
    int bettingRound(int &aBet, int &bBet, bool &aTurn);
    void playHand();
    void displayState(bool aTurn, int aBet, int bBet);

    int getDealt();
    int getAStack();
    int getBStack();
};

#endif