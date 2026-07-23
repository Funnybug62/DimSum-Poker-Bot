#include "card.h"
#include "game.h"

int main() {
    uint8_t deck[52];
    Card::buildDeck(deck);

    std::random_device rd;
    std::mt19937 gen(rd());

    Card::shuffle(deck, gen);
}