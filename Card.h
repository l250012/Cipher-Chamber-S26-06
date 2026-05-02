#ifndef CARD_H
#define CARD_H

#include <iostream>
#include <cstring>
#include "Player.h"

class Card
{
protected:
    int  difficulty;
    char suit[16];   // plain char array instead of std::string

public:
    Card(int difficulty, const char* suit);
    virtual ~Card();

    virtual void startChallenge(Player& player) = 0;

    int         getDifficulty() const;
    const char* getSuit()       const;
};

#endif // CARD_H
