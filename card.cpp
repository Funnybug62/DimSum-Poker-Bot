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

int getHandRank(Hand h, int freq[13], uint16_t rankMask) {
    int flush = Card::flush(h);
    if (flush != -1 && Card::straight(h.suits[flush])) return Card::STRAIGHTFLUSH;

    bool pair, twoPair, threeKind;
    pair = twoPair = threeKind = false;
    bool straight = Card::straight(rankMask);

    for (size_t i{0}; i < 13; ++i) {
        if (freq[i] == 4) return Card::FOURKIND;

        if (pair && freq[i] == 2) twoPair = true;

        if (freq[i] == 2) pair = true;
        if (freq[i] == 3) threeKind = true;
    }

    if (pair && threeKind) return Card::FULLHOUSE;
    if (flush != -1) return Card::FLUSH;
    if (straight) return Card::STRAIGHT;
    if (threeKind) return Card::THREEKIND;
    if (twoPair) return Card::TWOPAIR;
    if (pair) return Card::PAIR;

    return Card::HIGHCARD;
}

void getKickers(Hand h, int freq[13], uint16_t handMask, int handRank, int (&kickers)[5]) {
    switch (handRank) {
        case Card::STRAIGHTFLUSH: {
            if ((handMask & 1) != 0) {
                if ((handMask & 0b10) != 0) { kickers[0] = 5; return; }
                kickers[0] = 14;
                return;
            } else {
                for (int i{12}; i >= 0; --i) { 
                    if (handMask & (1 << i)) { kickers[0] = i; return; } 
                }
            }
            return;
        } 

        case Card::FOURKIND: {
            int nextBest = 0;
            for (int i{12}; i >= 0; --i) if (freq[i] == 4) { kickers[0] = i; break; }
            for (int i{12}; i >= 0; --i) if (freq[i] > 0 && i != kickers[0]) { nextBest = i; break; }
            kickers[1] = nextBest;
            return;
        }

        case Card::FULLHOUSE: {
            bool tripFound = false;
            bool pairFound = false;
            for (int i{12}; i >= 0 && !pairFound && !tripFound; --i) {
                if (!tripFound && freq[i] == 3) { kickers[0] = i; tripFound = true; }
                if (!pairFound && freq[i] >= 2) { kickers[1] = i; pairFound = true; }
            }
            
            return;
        }

        case Card::FLUSH: {
            int suit = Card::flush(h);
            for (int i{12}, toBeFound = 5; i >= 0 && toBeFound > 0; --i) 
                if ((h.suits[suit] & (1 << i)) != 0) { kickers[5-toBeFound] = i; toBeFound--; }
            return;
        } 

        case Card::STRAIGHT: {
            if ((handMask & 1) != 0) {
                if ((handMask & 0b10) != 0) { kickers[0] = 5; return; }
                kickers[0] = 14;
                return;
            } else {
                for (int i{12}; i >= 0; --i) { 
                    if (handMask & (1 << i)) { kickers[0] = i; return; } 
                }
            }
        }

        case Card::THREEKIND: { 
            for (int i{12}; i >= 0; --i) 
                if (freq[i] == 3) { kickers[0] = i; break; }
            
            for (int i{12}, toBeFound = 2; i >= 0 && toBeFound > 0; --i) 
                if (freq[i] > 0 && i != kickers[0]) { kickers[3-toBeFound] = i; toBeFound--; }
            return;
        } 

        case Card::TWOPAIR: {
            int highPair = -1;
            int lowPair = -1;
            for (int i{12}; i >= 0 && (highPair == -1 || lowPair == -1); --i) 
                if (highPair == -1 && freq[i] == 2) { highPair = i; }
                else if (freq[i] == 2) { lowPair = i; }
            
            kickers[0] = highPair;
            kickers[1] = lowPair;
            
            for (int i{12}; i >= 0; --i) 
                if (i != highPair && i != lowPair && freq[i] > 0) { kickers[2] = i; break; }
            return;
        }

        case Card::PAIR: {
            for (int i{12}; i >= 0; --i) 
                if (freq[i] == 2) { kickers[0] = i; break; }

            for (int i{12}, toBeFound = 3; i >= 0 && toBeFound > 0; --i) 
                if (freq[i] > 0 && i != kickers[0]) { kickers[4-toBeFound] = i; toBeFound--; }
            return;
        }

        case Card::HIGHCARD: {
            for (int i{12}, toBeFound = 5; i >= 0 && toBeFound > 0; --i) 
                if (freq[i] > 0) { kickers[5-toBeFound] = i; toBeFound--; }
            return;
        }
    }
}

uint32_t Card::getHandValue(Hand h) {
    int freq[13]{};
    buildFreq(h, freq);

    uint16_t rankMask = (h.suits[0] | h.suits[1] | h.suits[2] | h.suits[3]);

    int handRank = getHandRank(h, freq, rankMask);

    uint32_t value{0};

    value |= (handRank << 20);

    int kickers[5]{};
    getKickers(h, freq, rankMask, handRank, kickers);

    for (size_t i{0}; i < 5; i++) value |= (kickers[i] << (16-4*i));

    return value;
}

void Card::addCard(uint8_t card, Hand& h) {
    int rank = (int) (card & ((1 << 4) - 1));
    int suit = (int) (card >> 4);

    h.suits[suit] |= (1 << rank);
}