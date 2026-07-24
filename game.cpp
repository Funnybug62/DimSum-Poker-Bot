#include "game.h"

void Game::clearTerminal() { system("clear"); }

int Game::getDealt() { return dealt; }
int Game::getAStack() { return aStack; }
int Game::getBStack() { return bStack; }


std::string Game::getStageString() {
    switch (stage) {
        case PREFLOP: return "PREFLOP";
        case FLOP: return "FLOP";
        case TURN: return "TURN";
        case RIVER: return "RIVER";
        case SHOWDOWN: return "SHOWDOWN";
        default: return "N/A";
    }
}

void Game::printLastBotMove(int lastBotMove, int raiseAmount) {
    switch (lastBotMove) {
        case CHECK: {
            std::cout << "Bot: Checked" << std::endl;
            return;
        }

        case RAISE: {
            std::cout << "Bot: Raised " << raiseAmount << std::endl; 
            return;
        }

        case CALL: {
            std::cout << "Bot: Called" << std::endl;
            return;
        }

        case FOLD: {
            std::cout << "Bot: Folded" << std::endl;
            return;
        }
    }
}

void Game::displayState(bool aTurn, int aBet, int bBet) {
    clearTerminal();
    clearTerminal();

    std::cout << "STAGE: " << getStageString() << std::endl;
    if (aTurn) {
        if (lastBotMove != -1) printLastBotMove(lastBotMove, lastBotRaise);
        
        std::cout << "Current Turn: A" << std::endl << std::endl;
        std::cout << "Hole: ";
        Card::displayCard(holeA[0]);
        std::cout << "  ";
        Card::displayCard(holeA[1]);
    } else {
        std::cout << "Current Turn: B" << std::endl << std::endl;
        std::cout << "Hole: ";
        Card::displayCard(holeB[0]);
        std::cout << "  ";
        Card::displayCard(holeB[1]);
    }

    std::cout << std::endl << "Community Cards: ";
        for (size_t i{0}; i < communityDealt; ++i) {
            Card::displayCard(communityCards[i]);
            std::cout << "  ";
        }

        std::cout << std::endl << "Pot: " << pot << std::endl << std::endl;

        std::cout << "player A Stack: " << aStack-aBet << std::endl << "player B Stack: " << bStack-bBet << std::endl;
}

void Game::dealHand() { 
    Card::buildDeck(deck);
    Card::shuffle(deck, gen);

    A = Hand{};
    B = Hand{};
    
    dealt = 0;
    communityDealt = 0;
    for (size_t i{0}; i < 5; ++i) communityCards[i] = 0;
    pot = 0;

    holeA[0] = deck[dealt++];
    holeA[1] = deck[dealt++];

    int hole1 = Card::getRank(holeA[0]);
    int hole2 = Card::getRank(holeA[1]);
 
    if (hole2 < hole1) {
        uint8_t temp = holeA[0];
        holeA[0] = holeA[1];
        holeA[1] = temp;
    }

    holeB[0] = deck[dealt++];
    holeB[1] = deck[dealt++];

    hole1 = Card::getRank(holeB[0]);
    hole2 = Card::getRank(holeB[1]);
 
    if (hole2 < hole1) {
        uint8_t temp = holeB[0];
        holeB[0] = holeB[1];
        holeB[1] = temp;
    }
}

int Game::getAction(int opposingBet, int curBet, bool invalid) {
    if (!invalid) {
        std::cout << "Other Player Bet: " << opposingBet << std::endl;
        std::cout << "You are betting: " << curBet << std::endl << std::endl;
    }
    std::cout << "1: CHECK | 2: RAISE | 3: CALL | 4: FOLD" << std::endl;
    std::cout << "What to do: ";
    int nextAction;
    std::cin >> nextAction;

    return nextAction;
}

int Game::processAction(int action, int &aBet, int &bBet, bool &lastCheck, bool aTurn, double predictedWinRate) {
    switch (action) {
        case RAISE: {
            int raiseAmount; 
            
            int baseLine = (pot == 0) ? aBet + bBet : pot;
            if (!aTurn) lastBotRaise = raiseAmount = std::max(std::min(std::max((int)(baseLine * (0.3 + 0.7 * predictedWinRate)), (std::abs(aBet - bBet))), bStack-bBet), 0);
            else {
                std::cout << "Raise Amount: ";
                std::cin >> raiseAmount;
            }

            while ((raiseAmount > ((aTurn) ? aStack : bStack)) || raiseAmount <= 0 || (raiseAmount + ((aTurn) ? aBet : bBet)) < ((aTurn) ? bBet : aBet)) {
                std::cout << "Raise Amount was: " << raiseAmount;
                std::cout << "Invalid Raise Amount. Try Again" << std::endl;
                std::cout << "Raise Amount: ";
                std::cin >> raiseAmount;
            }

            if (aTurn) aBet += raiseAmount;
            else bBet += raiseAmount;
            
            return 1;
        }

        case CHECK: {
            if (lastCheck) return 0;
            lastCheck = true;
            return 1;
        }

        case CALL: {
            if (aTurn) aBet = std::min(bBet, aStack);
            else bBet = std::min(aBet, bStack);
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
        if (aTurn) displayState(aTurn, aBet, bBet);
        
        canCheck = false;
        canCall = false;
        int opposingBet = (aTurn) ? bBet : aBet;
        int curBet = (aTurn) ? aBet : bBet;
        int action;
        double predictedWinRate = Engine::estimateWR(holeB, communityCards, communityDealt, 250000);

        if (aTurn) action = getAction(opposingBet, curBet, false);
        else {
            int callAmount = std::abs(opposingBet - curBet);
            action = Engine::makeDecision(predictedWinRate, pot, callAmount, bStack-bBet);
            lastBotMove = action;
        }

        if (aBet == bBet) canCheck = true;
        if (aBet != bBet) canCall = true;
        
        while (aTurn && ((action == CHECK && !canCheck) || (action == CALL && !canCall))) {
            std::cout << "invalid move" << std::endl;

            action = getAction(opposingBet, curBet, true);
        }

        int result = processAction(action, aBet, bBet, lastCheck, aTurn, predictedWinRate);

        if (result == -1) return -1;
        if (result == 0) break;
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

void Game::endGame(uint8_t holeA[2], uint8_t holeB[2], uint32_t aHandVal, uint32_t bHandVal) {
    std::cout << "A Hand: ";
    Card::displayCard(holeA[0]);
    Card::displayCard(holeA[1]);
    std::cout << std::endl;

    std::cout << "B Hand: ";
    Card::displayCard(holeB[0]);
    Card::displayCard(holeB[1]);
    std::cout << std::endl;

    if (aHandVal == bHandVal) { 
        aStack += pot/2; 
        bStack += pot/2; 

        std::cout << "Tie" << std::endl;
    } else if (aHandVal > bHandVal) {
        aStack += pot;

        std::cout << "A Win" << std::endl;
    } else {
        bStack += pot;

        std::cout << "B Win" << std::endl;
    }

    std::cout << "Enter to continue..";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    std::cin.get();
}

void Game::playHand() {
    dealHand();

    int aBet = (dealer == 0) ? 10 : 20;
    int bBet = (dealer == 0) ? 20 : 10;
    bool aTurn = (dealer == 0) ? true : false;
    int betRound;

    stage = PREFLOP;
    
    // preflop
    betRound = bettingRound(aBet, bBet, aTurn);
    handleBet(aBet, bBet);

    if (betRound == -1) {
        if (aTurn) bStack += pot;
        else aStack += pot;

        std::cout << "A Hand: ";
        Card::displayCard(holeA[0]);
        Card::displayCard(holeA[1]);
        std::cout << std::endl;

        std::cout << "B Hand: ";
        Card::displayCard(holeB[0]);
        Card::displayCard(holeB[1]);
        std::cout << std::endl;

        std::cout << "Enter to continue..";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
        std::cin.get();
        return;
    }

    stage = FLOP;

    // flop
    for (size_t i{0}; i < 3; ++i) communityCards[i] = deck[dealt++];
    communityDealt = 3;
    
    bool mover = (dealer == 0) ? false : true;
    betRound = bettingRound(aBet, bBet, mover);
    handleBet(aBet, bBet);

    if (betRound == -1) {
        if (mover) bStack += pot;
        else aStack += pot;

        std::cout << "A Hand: ";
        Card::displayCard(holeA[0]);
        Card::displayCard(holeA[1]);
        std::cout << std::endl;

        std::cout << "B Hand: ";
        Card::displayCard(holeB[0]);
        Card::displayCard(holeB[1]);
        std::cout << std::endl;

        std::cout << "Enter to continue..";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
        std::cin.get();
        return;
    }

    stage = TURN;

    // turn
    communityCards[3] = deck[dealt++];
    communityDealt = 4;

    mover = (dealer == 0) ? false : true;
    betRound = bettingRound(aBet, bBet, mover);
    handleBet(aBet, bBet);

    if (betRound == -1) {
        if (mover) bStack += pot;
        else aStack += pot;

        std::cout << "A Hand: ";
        Card::displayCard(holeA[0]);
        Card::displayCard(holeA[1]);
        std::cout << std::endl;

        std::cout << "B Hand: ";
        Card::displayCard(holeB[0]);
        Card::displayCard(holeB[1]);
        std::cout << std::endl;

        std::cout << "Enter to continue..";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
        std::cin.get();
        return;
    }

    stage = RIVER;
    
    // river
    communityCards[4] = deck[dealt++];
    communityDealt = 5;

    mover = (dealer == 0) ? false : true;
    betRound = bettingRound(aBet, bBet, mover);
    handleBet(aBet, bBet);

    if (betRound == -1) {
        if (mover) bStack += pot;
        else aStack += pot;

        std::cout << "A Hand: ";
        Card::displayCard(holeA[0]);
        Card::displayCard(holeA[1]);
        std::cout << std::endl;

        std::cout << "B Hand: ";
        Card::displayCard(holeB[0]);
        Card::displayCard(holeB[1]);
        std::cout << std::endl;

        std::cout << "Enter to continue..";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
        std::cin.get();
        return;
    }

    stage = SHOWDOWN;

    // showdown

    for (size_t i{0}; i < 5; ++i) { Card::addCard(communityCards[i], A); Card::addCard(communityCards[i], B); } 
    for (size_t i{0}; i < 2; ++i) { Card::addCard(holeA[i], A); Card::addCard(holeB[i], B); }

    uint32_t aHandVal = Card::getHandValue(A);
    uint32_t bHandVal = Card::getHandValue(B);

    endGame(holeA, holeB, aHandVal, bHandVal);

    dealer ^= 1;
}

Game::Game() : 
    A{{}}, B{{}}, aStack{1000}, bStack{1000}, stage{0}, pot{0}, communityDealt{0}, lastBotMove{-1},
    dealer{0}, communityCards{0}, holeA{0}, holeB{0}, dealt{0}, gen(rd())
{

}