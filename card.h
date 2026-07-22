#ifndef CARD_H
#define CARD_H

#include <iostream>
#include <random>

struct Hand {
    uint16_t suits[4];
};

namespace Card {
    inline const char* ranks[13] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    inline const char* suits[4] = {"♠", "♥", "♦", "♣"};

    void shuffle(uint8_t (&deck)[52], std::mt19937& gen);
    void printCard(uint8_t card);
    void buildDeck(uint8_t (&deck)[52]);
    void addCard(uint8_t card, Hand& h);
    int flush(Hand h);
    bool straight(uint16_t rankMask);
    uint32_t getHandValue(Hand h);
};

#endif