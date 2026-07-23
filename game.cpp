#include "game.h"

void Game::dealHand() { 
    Card::buildDeck(deck);
    Card::shuffle(deck, gen);

    A = Hand{};
    B = Hand{};
    
    dealt = 0;
    pot = 30;
    aStack -= (dealer == 0) ? smallBlind : bigBlind;
    bStack -= (dealer == 1) ? smallBlind : bigBlind;

    holeA[0] = deck[dealt++];
    holeA[1] = deck[dealt++];
    holeB[0] = deck[dealt++];
    holeB[1] = deck[dealt++];
    
    stage = PREFLOP;
}

int Game::getAction() {
    std::cout << "1: CHECK | 2: RAISE | 3: CALL | 4: FOLD" << std::endl;
    std::cout << "What to do: ";
    int nextAction;
    std::cin >> nextAction;

    return nextAction;
}

int Game::processAction(int action, int &aBet, int &bBet, bool &lastCheck, bool aTurn) {
    switch (action) {
        case CHECK: {
            if (lastCheck) return 0;
            lastCheck = true;
            return 1;
        }

        case RAISE: {
            int raiseAmount;
            std::cout << "Raise Amount: ";
            std::cin >> raiseAmount;

            if (aTurn) aBet += raiseAmount;
            else bBet += raiseAmount;
            
            return 1;
        }

        case CALL: {
            if (aTurn) aBet = bBet;
            else bBet = aBet;
            return 0;
        }

        case FOLD: {
            return -1;
        }
    }
}

int Game::bettingRound(int &aBet, int &bBet, bool &aTurn) {
    bool lastCheck = false;
    bool canCheck = false;
    bool canCall = false; 

    while (true) {
        int action = getAction();
        if (aBet == bBet) canCheck = true;
        if (aBet != bBet) canCall = true;
        
        if ((action == CHECK && !canCheck) || (action == CALL && !canCall)) {
            std::cout << "invalid move" << std::endl;
            continue;
        }

        int result = processAction(action, aBet, bBet, lastCheck, aTurn);

        if (result == -1) return -1;
        if (result == 0) break;

        canCheck = false;
        canCall = false;
        aTurn ^= true;
    }

    return 1;
}

void Game::handleBet(int &aBet, int &bBet) {
    pot += aBet;
    pot += bBet;

    aStack -= aBet;
    bStack -= bBet;

    aBet = 0;
    bBet = 0;
}

void Game::playHand() {
    dealHand();

    int aBet = (dealer == 0) ? 10 : 20;
    int bBet = (dealer == 0) ? 20 : 10;
    bool aTurn = (dealer == 0) ? true : false;
    int betRound;
    
    // preflop
    betRound = bettingRound(aBet, bBet, aTurn);
    handleBet(aBet, bBet);

    if (betRound == -1) {
        if (aTurn) bStack += pot;
        else aStack += pot;

        return;
    }

    // flop
    for (size_t i{0}; i < 3; ++i) communityCards[i] = deck[dealt++];
    
    bool mover = (dealer == 0) ? false : true;
    betRound = bettingRound(aBet, bBet, mover);
    handleBet(aBet, bBet);

    if (betRound == -1) {
        if (mover) bStack += pot;
        else aStack += pot;

        return;
    }

    // turn
    communityCards[3] = deck[dealt++];

    mover = (dealer == 0) ? false : true;
    betRound = bettingRound(aBet, bBet, mover);
    handleBet(aBet, bBet);

    if (betRound == -1) {
        if (mover) bStack += pot;
        else aStack += pot;

        return;
    }
    
    // river
    communityCards[4] = deck[dealt++];

    mover = (dealer == 0) ? false : true;
    betRound = bettingRound(aBet, bBet, mover);
    handleBet(aBet, bBet);

    if (betRound == -1) {
        if (mover) bStack += pot;
        else aStack += pot;

        return;
    }

    for (size_t i{0}; i < 5; ++i) { Card::addCard(communityCards[i], A); Card::addCard(communityCards[i], B); } 
    for (size_t i{0}; i < 2; ++i) { Card::addCard(holeA[i], A); Card::addCard(holeB[i], B); }

    uint32_t aHandVal = Card::getHandValue(A);
    uint32_t bHandVal = Card::getHandValue(B);

    if (aHandVal == bHandVal) { aStack += pot/2; bStack += pot/2; }
    else if (aHandVal > bHandVal) aStack += pot;
    else bStack += pot;

    dealer ^= 1;
}


Game::Game() : 
    A{{}}, B{{}}, aStack{1000}, bStack{1000}, stage{0}, pot{0}, 
    dealer{0}, communityCards{-1}, holeA{-1}, holeB{-1}, dealt{0}, gen(rd())
{
    dealHand();
}