#include "card.h"
#include "game.h"
#include "engine.h"

void playFullGame(Game &g) {
    while (g.getAStack() > 0 && g.getBStack() > 0) g.playHand();
    
    if (g.getAStack() > 0) std::cout << "A Won" << std::endl;
    else std::cout << "B Won" << std::endl;
}

int main() {
    Game g;

    playFullGame(g);
}