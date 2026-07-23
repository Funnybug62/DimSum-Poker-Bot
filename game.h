#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <random>
#include "card.h"

class Game {
private:
    const int PREFLOP = 0;
    const int FLOP = 1;
    const int TURN = 2;
    const int RIVER = 3;
    const int SHOWDOWN = 4;

    static constexpr int CHECK = 0;
    static constexpr int RAISE = 1;
    static constexpr int CALL = 2;
    static constexpr int FOLD = 3;

    std::random_device rd;
    std::mt19937 gen;

    Hand A;
    Hand B;

    int aStack;
    int bStack;

    int stage;
    int pot;
    int dealer; // 0 for A, 1 for B

    uint8_t communityCards[5];
    uint8_t holeA[2];
    uint8_t holeB[2];   

    int dealt;
    uint8_t deck[52];

    int getAction();
    int processAction(int action, int &aBet, int &bBet, bool &lastCheck, bool aTurn);
    void handleBet(int &aBet, int &bBet);
public:
    const int smallBlind = 10;
    const int bigBlind = 20;

    Game();
    void dealHand();
    int bettingRound(int &aBet, int &bBet, bool &aTurn);
    void playHand();
};

#endif