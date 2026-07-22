#include <iostream>
#include <random>

void buildDeck(uint8_t (&deck)[52]) {
    for (size_t i{0}; i < 4; ++i) {
        for (size_t j{0}; j < 13; ++j) {
            uint8_t card = (i << 4) | j;

            deck[i * 13 + j] = card;
        }
    }
}

void swap(int a, int b, uint8_t (&deck)[52]) {
    uint8_t temp = deck[a];
    deck[a] = deck[b];
    deck[b] = temp; 
}

void shuffle(uint8_t (&deck)[52], std::mt19937& gen) {
    for (size_t i{51}; i > 0; --i) {
        std::uniform_int_distribution<int> distrib(0, i);
        int randInd = distrib(gen);

        swap(randInd, i, deck);
    }   
}

void printCard(uint8_t card, const char* ranks[], const char* suits[]) {
    uint8_t suit = card >> 4;
    uint8_t rank = card & ((1 << 4)-1);

    std::cout << suits[suit] << ranks[rank] << std::endl;
} 

int main() {
    const char* ranks[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    const char* suits[] = {"♠", "♥", "♦", "♣"};

    uint8_t deck[52];
    buildDeck(deck);

    std::random_device rd;
    std::mt19937 gen(rd());

    shuffle(deck, gen);

    printCard((uint8_t) 0b00110011, ranks, suits);
}   