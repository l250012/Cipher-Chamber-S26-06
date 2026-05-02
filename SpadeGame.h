#ifndef SPADEGAME_H
#define SPADEGAME_H

#include <iostream>
#include <cstring>
#include <ctime>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "Card.h"
#include "Player.h"

static const int MAX_COLS = 32;
static const int MAX_ROWS = 32;

// ─────────────────────────────────────────────
//  Cell — one maze tile
// ─────────────────────────────────────────────
struct Cell
{
    bool wallTop;
    bool wallRight;
    bool wallBottom;
    bool wallLeft;
};

// ─────────────────────────────────────────────
//  Pos — manual DFS stack entry
// ─────────────────────────────────────────────
struct Pos { int c; int r; };

// ─────────────────────────────────────────────
//  SpadeGame — SFML maze survival challenge
//  Inherits from abstract Card base class
// ─────────────────────────────────────────────
class SpadeGame : public Card
{
private:

    // ── Maze ──────────────────────────────────
    int  cols;
    int  rows;
    int  cellSize;
    Cell maze[MAX_COLS][MAX_ROWS];
    bool visited[MAX_COLS][MAX_ROWS];

    // ── DFS stack (no std::stack) ─────────────
    Pos  dfsStack[MAX_COLS * MAX_ROWS];
    int  stackTop;

    // ── Player ────────────────────────────────
    int  playerCol;
    int  playerRow;

    // ── State ─────────────────────────────────
    bool    won;
    bool    gameOver;
    float   timeLimit;
    float   timeLeft;

    // ── SFML ──────────────────────────────────
    sf::RenderWindow window;
    sf::Clock        clock;
    sf::Font         font;
    bool             fontLoaded;

    // ── Helpers ───────────────────────────────
    void initMaze();
    void generateMaze();

    void render(Player& player);
    void drawMaze();
    void drawPlayer();
    void drawExit();
    void drawHUD(Player& player);
    void drawEndScreen(bool playerWon);

    void handleInput();

    // itoa replacement using cstring only
    void intToStr(int val, char* buf) const;

public:
    SpadeGame(int difficulty);
    ~SpadeGame();

    void startChallenge(Player& player) override;
    void displayInfo() const;
};

#endif // SPADEGAME_H
