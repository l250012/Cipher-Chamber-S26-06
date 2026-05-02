#include "Player.h"

Player::Player(const char* name, int health, int intelligence, int stamina)
    : health(health), intelligence(intelligence), stamina(stamina)
{
    std::strncpy(this->name, name, 31);
    this->name[31] = '\0';
    std::cout << "[Player] " << this->name
              << " has entered the Borderlands.\n";
}

void Player::move(float x, float y)
{
    std::cout << "[Player] " << name << " moves to ("
              << x << ", " << y << ").\n";
}

void Player::takeDamage(int dmg)
{
    health -= dmg;
    if (health < 0) health = 0;
    std::cout << "[Player] " << name << " took " << dmg
              << " damage! HP remaining: " << health << "\n";
}

void Player::heal(int amount)
{
    health += amount;
    if (health > 100) health = 100;
    std::cout << "[Player] " << name << " healed " << amount
              << " HP. HP now: " << health << "\n";
}

int         Player::getHealth()       const { return health;       }
int         Player::getIntelligence() const { return intelligence; }
int         Player::getStamina()      const { return stamina;      }
const char* Player::getName()         const { return name;         }

void Player::displayStatus() const
{
    std::cout << "==============================\n";
    std::cout << "  Player  : " << name         << "\n";
    std::cout << "  Health  : " << health       << "/100\n";
    std::cout << "  Intel   : " << intelligence << "\n";
    std::cout << "  Stamina : " << stamina      << "\n";
    std::cout << "==============================\n";
}

bool Player::isAlive() const { return health > 0; }
