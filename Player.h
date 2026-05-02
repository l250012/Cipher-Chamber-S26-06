#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <cstring>

class Player
{
private:
    char name[32];
    int  health;
    int  intelligence;
    int  stamina;

public:
    Player(const char* name = "Arisu", int health = 100,
           int intelligence = 80,      int stamina = 100);

    void move(float x, float y);
    void takeDamage(int dmg);
    void heal(int amount);

    int         getHealth()       const;
    int         getIntelligence() const;
    int         getStamina()      const;
    const char* getName()         const;

    void displayStatus() const;
    bool isAlive()        const;
};

#endif // PLAYER_H
