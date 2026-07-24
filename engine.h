#ifndef ENGINE_H
#define ENGINE_H

#include "card.h"
#include "iostream"

class Engine {
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

    // std::random_device rd;
    // std::mt19937 gen;

    static void shuffle(std::vector<uint8_t> deck, std::mt19937);
    static bool containsCard(uint8_t card, uint8_t holeCards[2], uint8_t communityCards[5]);
    static void buildUnknowns(std::vector<uint8_t> (&unknowns), uint8_t holeCards[2], uint8_t communityCards[5]);
public:
    Engine();

    static int makeDecision(double winRate, int pot, int callAmount, int stack);
    static double estimateWR(uint8_t holeCards[2], uint8_t communityCards[5], int dealt, int numSims); 
};  

#endif ENGINE_H