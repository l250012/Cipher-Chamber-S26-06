#pragma once
//#include <SFML/Graphics.hpp>

class Player {
private:
    int health;
    int intelligence;
    int stamina;
    /*sf::Sprite sprite;*/

public:
    Player() : health(100), intelligence(50), stamina(50) {}

   /* void setTexture(sf::Texture& texture) {
        sprite.setTexture(texture);
    }

    void move(float x, float y) {
        sprite.move(x, y);
    }*/

    void takeDamage(int dmg) {
        health -= dmg;
        if (health < 0) health = 0;
    }

    void heal(int amount) {
        health += amount;
        if (health > 100) health = 100;
    }

    int getHealth() const {
        return health;
    }

   /* sf::Sprite& getSprite() {
        return sprite;
    }*/
};