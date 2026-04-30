#pragma once
#include "playerclass.h"

class HealthSystem {
public:
    static void applyDamage(Player& player, int dmg) {
        player.takeDamage(dmg);
    }

    static void regenerate(Player& player) {
        player.heal(1); // simple regen
    }
};