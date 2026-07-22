#include "card.h"

void Card::buildDeck(uint8_t (&deck)[52]) {
    for (size_t i{0}; i < 4; ++i) {
        for (size_t j{0}; j < 13; ++j) {
            uint8_t card = (i << 4) | j;

            deck[i * 13 + j] = card;
        }
    }
}

static void swap(int a, int b, uint8_t (&deck)[52]) {
    uint8_t temp = deck[a];
    deck[a] = deck[b];
    deck[b] = temp; 
}

void Card::shuffle(uint8_t (&deck)[52], std::mt19937& gen) {
    for (size_t i{51}; i > 0; --i) {
        std::uniform_int_distribution<int> distrib(0, i);
        int randInd = distrib(gen);

        swap(randInd, i, deck);
    }   
}

void Card::printCard(uint8_t card) {
    uint8_t suit = card >> 4;
    uint8_t rank = card & ((1 << 4)-1);

    std::cout << Card::suits[suit] << Card::ranks[rank] << std::endl;
} 

int Card::flush(Hand h) {
    for (size_t i{0}; i < 4; ++i) if (__builtin_popcount(h.suits[i]) >= 5) return i;
    return -1;
}

bool Card::straight(uint16_t rankMask) {
    if (rankMask & 1) rankMask |= (1 << 13);

    return ((rankMask & (rankMask >> 1) & (rankMask >> 2) & (rankMask >> 3) & (rankMask >> 4)) != 0);
}

void buildFreq(Hand h, int (&freq)[13]) {
    for (size_t i{0}; i < 4; ++i) 
        for (size_t j{0}; j < 13; ++j) if ((h.suits[i] & (1 << j)) != 0) freq[j]++;
}

uint32_t Card::getHandValue(Hand h) {
    int freq[13]{};
    buildFreq(h, freq);

    
}

void Card::addCard(uint8_t card, Hand& h) {
    int rank = (int) (card & ((1 << 4) - 1));
    int suit = (int) (card >> 4);

    h.suits[suit] |= (1 << rank);
}