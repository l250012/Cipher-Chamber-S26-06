#include "Card.h"

Card::Card(int difficulty, const char* suit)
    : difficulty(difficulty)
{
    std::strncpy(this->suit, suit, 15);
    this->suit[15] = '\0';
}

Card::~Card() {}

int         Card::getDifficulty() const { return difficulty; }
const char* Card::getSuit()       const { return suit;       }
