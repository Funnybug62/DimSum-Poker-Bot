#include "engine.h"

static void swap(int a, int b, std::vector<uint8_t> &deck) {
    uint8_t temp = deck[a];
    deck[a] = deck[b];
    deck[b] = temp;
}

void Engine::shuffle(std::vector<uint8_t> &deck, std::mt19937 &gen) {
    for (size_t i{deck.size()-1}; i > 0; --i) {
        std::uniform_int_distribution<int> dist(0, i);
        int randInd = dist(gen);

        swap(randInd, i, deck);
    }
}

void Engine::recordAction(int action, int stage) { actionFreq[stage][action]++; }

bool Engine::isUnknown(uint8_t card, uint8_t holeCards[2], uint8_t communityCards[5]) {
    return (card != holeCards[0] && card != holeCards[1] && card != communityCards[0] && card != communityCards[1] && card != communityCards[2] && card != communityCards[3] && card != communityCards[4]);
}

void Engine::buildUnknowns(std::vector<uint8_t> (&unknowns), uint8_t holeCards[2], uint8_t communityCards[5]) {
    for (size_t i{0}; i < 4; ++i) {
        for (size_t j{0}; j < 13; ++j) {
            uint8_t card = (i << 4) | j;

            if (isUnknown(card, holeCards, communityCards)) unknowns.push_back(card);
        }
    }
}

double Engine::estimateWR(uint8_t holeCards[2], uint8_t communityCards[5], int dealt, int numSims) {
    std::vector<uint8_t> unknowns;
    uint8_t communityCopy[5];
    for (size_t i{0}; i < 5; ++i) communityCopy[i] = communityCards[i];

    buildUnknowns(unknowns, holeCards, communityCards);

    std::random_device rd;
    std::mt19937 gen(rd());

    int wins = 0;

    for (size_t sim{0}; sim < numSims; ++sim) {
        shuffle(unknowns, gen);

        int ind = 0;

        uint8_t opponentHole[] = {unknowns[ind++], unknowns[ind++]};

        for (int i{dealt}; i < 5; ++i) {
            communityCopy[i] = unknowns[ind++];
        }

        Hand A{}, B{};
        for (size_t i{0}; i < 5; ++i) { Card::addCard(communityCopy[i], A); Card::addCard(communityCopy[i], B); } 
        for (size_t i{0}; i < 2; ++i) { Card::addCard(holeCards[i], A); Card::addCard(opponentHole[i], B); }

        if (Card::getHandValue(A) > Card::getHandValue(B)) wins++;
    }

    return (double) wins/numSims;
}

int Engine::makeDecision(double winRate, int pot, int callAmount, int stack) {
    double potOdds = (double) callAmount / (pot + callAmount);

    int bluffNum = (std::rand() % (10)) + 1;

    if (stack > 0 && winRate > 0.85) return RAISE;
    if (stack > 0 && bluffNum == 1) return RAISE;

    if (callAmount == 0) return CHECK;
    if (stack >= callAmount && winRate > 0.65) return CALL;
    if (winRate < potOdds) return FOLD;

    return CALL;
}

Engine::Engine() {

}