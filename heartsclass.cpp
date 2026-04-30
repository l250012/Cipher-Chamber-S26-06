//#pragma once
//#include <SFML/Graphics.hpp>
//#include <cstdlib>
//#include <ctime>
//#include "Playerclass.h"
//using namespace std;
//
//class HeartGame {
//private:
//    sf::Texture bgTex, npcTex;
//    sf::Sprite background;
//    sf::Sprite npcs[3];
//
//    sf::Font font;
//    sf::Text message;
//
//    sf::RectangleShape bubble;
//    sf::Text bubbleText;
//
//    int imposterIndex;
//    bool gameOver;
//
//public:
//    HeartGame() {
//        srand((unsigned)time(0));
//        imposterIndex = rand() % 3;
//        gameOver = false;
//    }
//
//    // ---------------- LOAD ----------------
//    void loadAssets() {
//        bgTex.loadFromFile("assets/court_bg.png");
//        npcTex.loadFromFile("assets/npc.png");
//        font.loadFromFile("assets/arial.ttf");
//
//        background.setTexture(bgTex);
//
//        for (int i = 0; i < 3; i++) {
//            npcs[i].setTexture(npcTex);
//            npcs[i].setPosition(200 + i * 200, 300);
//        }
//
//        bubble.setFillColor(sf::Color(0, 0, 0, 180));
//
//        bubbleText.setFont(font);
//        bubbleText.setCharacterSize(14);
//        bubbleText.setFillColor(sf::Color::White);
//
//        message.setFont(font);
//        message.setCharacterSize(20);
//        message.setPosition(50, 50);
//        message.setString("Find the liar ");
//    }
//
//    // ---------------- BUBBLE ----------------
//    void updateBubble(sf::Vector2f mousePos) {
//        for (int i = 0; i < 3; i++) {
//            if (npcs[i].getGlobalBounds().contains(mousePos)) {
//
//                bubble.setPosition(npcs[i].getPosition().x, npcs[i].getPosition().y - 40);
//                bubbleText.setPosition(bubble.getPosition().x + 5, bubble.getPosition().y + 5);
//
//                bubble.setSize(sf::Vector2f(120, 25));
//
//                if (i == imposterIndex)
//                    bubbleText.setString("I'm innocent");
//                else
//                    bubbleText.setString("Trust me!");
//
//                return;
//            }
//        }
//
//        bubble.setSize(sf::Vector2f(0, 0));
//    }
//
//    // ---------------- CLICK ----------------
//    void handleClick(sf::Vector2f mousePos, Player& player) {
//        if (gameOver) return;
//
//        for (int i = 0; i < 3; i++) {
//            if (npcs[i].getGlobalBounds().contains(mousePos)) {
//
//                if (i == imposterIndex) {
//                    message.setString("Correct ");
//                    player.heal(10);
//                }
//                else {
//                    message.setString("Wrong ");
//                    player.takeDamage(10);
//                }
//
//                gameOver = true;
//            }
//        }
//    }
//
//    // ---------------- RENDER ----------------
//    void render(sf::RenderWindow& window) {
//        window.draw(background);
//
//        for (int i = 0; i < 3; i++)
//            window.draw(npcs[i]);
//
//        window.draw(bubble);
//        window.draw(bubbleText);
//        window.draw(message);
//    }
//};