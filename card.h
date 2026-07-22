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

    inline const int STRAIGHTFLUSH = 8;
    inline const int FOURKIND = 7;
    inline const int FULLHOUSE = 6;
    inline const int FLUSH = 5;
    inline const int STRAIGHT = 4;
    inline const int THREEKIND = 3;
    inline const int TWOPAIR = 2;
    inline const int PAIR = 1;
    inline const int HIGHCARD = 0;

    void shuffle(uint8_t (&deck)[52], std::mt19937& gen);
    void printCard(uint8_t card);
    void buildDeck(uint8_t (&deck)[52]);
    void addCard(uint8_t card, Hand& h);
    int flush(Hand h);
    bool straight(uint16_t rankMask);
    uint32_t getHandValue(Hand h);
};

#endif