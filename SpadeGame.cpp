#include "SpadeGame.h"

// ═══════════════════════════════════════════════════════════
//  intToStr — convert int to c-string (no sstream/sprintf)
// ═══════════════════════════════════════════════════════════
void SpadeGame::intToStr(int val, char* buf) const
{
    if (val == 0) { buf[0] = '0'; buf[1] = '\0'; return; }

    char tmp[16];
    int  i   = 0;
    bool neg = (val < 0);
    if (neg) val = -val;

    while (val > 0)
    {
        tmp[i++] = '0' + (val % 10);
        val /= 10;
    }
    if (neg) tmp[i++] = '-';

    // reverse into buf
    int len = i;
    for (int j = 0; j < len; ++j)
        buf[j] = tmp[len - 1 - j];
    buf[len] = '\0';
}

// ═══════════════════════════════════════════════════════════
//  Constructor
// ═══════════════════════════════════════════════════════════
SpadeGame::SpadeGame(int difficulty)
    : Card(difficulty, "Spade"),
      cols(0), rows(0), cellSize(0),
      stackTop(0),
      playerCol(0), playerRow(0),
      won(false), gameOver(false),
      timeLimit(0.f), timeLeft(0.f),
      fontLoaded(false)
{
    // Scale with difficulty (1=easy → 13=king)
    cols      = 5 + difficulty * 2;              // 7 → 31
    rows      = 5 + difficulty * 2;
    cellSize  = (cols > 20) ? 20 : 640 / cols;  // auto-fit cell px
    timeLimit = 90.f - difficulty * 5.f;
    if (timeLimit < 20.f) timeLimit = 20.f;
    timeLeft  = timeLimit;

    // Seed RNG using ctime
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    initMaze();
    generateMaze();

    window.create(sf::VideoMode(800, 700),
                  "Alice in Borderland - Spade Challenge",
                  sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // Font paths — plain char arrays (no std::string, no vector)
    const char* paths[4] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSansBold.ttf",
        "C:/Windows/Fonts/arial.ttf"
    };
    for (int i = 0; i < 4; ++i)
        if (font.loadFromFile(paths[i])) { fontLoaded = true; break; }

    std::cout << "[SpadeGame] Maze " << cols << "x" << rows
              << "  |  Difficulty " << difficulty
              << "  |  Time " << timeLimit << "s\n";
}

// ═══════════════════════════════════════════════════════════
//  Destructor
// ═══════════════════════════════════════════════════════════
SpadeGame::~SpadeGame()
{
    if (window.isOpen()) window.close();
}

// ═══════════════════════════════════════════════════════════
//  initMaze — all walls up, nothing visited
//  Uses memset via cstring
// ═══════════════════════════════════════════════════════════
void SpadeGame::initMaze()
{
    for (int c = 0; c < MAX_COLS; ++c)
        for (int r = 0; r < MAX_ROWS; ++r)
        {
            maze[c][r].wallTop    = true;
            maze[c][r].wallRight  = true;
            maze[c][r].wallBottom = true;
            maze[c][r].wallLeft   = true;
            visited[c][r]         = false;
        }
}

// ═══════════════════════════════════════════════════════════
//  generateMaze — iterative DFS (recursive backtracker)
//  Manual Pos[] stack — no std::stack, no vector
// ═══════════════════════════════════════════════════════════
void SpadeGame::generateMaze()
{
    // Direction offsets: 0=Up 1=Right 2=Down 3=Left
    const int dc[4] = {  0,  1,  0, -1 };
    const int dr[4] = { -1,  0,  1,  0 };

    visited[0][0]     = true;
    stackTop          = 0;
    dfsStack[stackTop++] = {0, 0};

    while (stackTop > 0)
    {
        Pos cur = dfsStack[stackTop - 1];

        // Collect unvisited neighbours
        int dirs[4];
        int dirCount = 0;
        for (int d = 0; d < 4; ++d)
        {
            int nc = cur.c + dc[d];
            int nr = cur.r + dr[d];
            if (nc >= 0 && nc < cols &&
                nr >= 0 && nr < rows &&
                !visited[nc][nr])
                dirs[dirCount++] = d;
        }

        if (dirCount == 0) { --stackTop; continue; }

        // Pick random direction
        int d  = dirs[std::rand() % dirCount];
        int nc = cur.c + dc[d];
        int nr = cur.r + dr[d];

        // Carve wall between current cell and neighbour
        switch (d)
        {
            case 0: maze[cur.c][cur.r].wallTop    = false;
                    maze[nc][nr].wallBottom        = false; break;
            case 1: maze[cur.c][cur.r].wallRight   = false;
                    maze[nc][nr].wallLeft           = false; break;
            case 2: maze[cur.c][cur.r].wallBottom  = false;
                    maze[nc][nr].wallTop            = false; break;
            case 3: maze[cur.c][cur.r].wallLeft    = false;
                    maze[nc][nr].wallRight          = false; break;
        }

        visited[nc][nr]      = true;
        dfsStack[stackTop++] = {nc, nr};
    }
}

// ═══════════════════════════════════════════════════════════
//  startChallenge — main SFML game loop
// ═══════════════════════════════════════════════════════════
void SpadeGame::startChallenge(Player& player)
{
    std::cout << "[SpadeGame] Challenge started for "
              << player.getName() << "\n";
    clock.restart();

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        timeLeft -= dt;

        // ── Events ────────────────────────────
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (gameOver &&
                event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Return)
                window.close();
        }

        // ── Logic ─────────────────────────────
        if (!gameOver)
        {
            handleInput();

            if (timeLeft <= 0.f)
            {
                timeLeft = 0.f;
                won      = false;
                gameOver = true;
                player.takeDamage(30 * difficulty);
                std::cout << "[SpadeGame] Time out! "
                          << player.getName() << " failed.\n";
            }

            if (playerCol == cols - 1 && playerRow == rows - 1)
            {
                won      = true;
                gameOver = true;
                std::cout << "[SpadeGame] "
                          << player.getName()
                          << " escaped the maze!\n";
            }
        }

        render(player);
    }

    if (!won) player.takeDamage(20 * difficulty);
    player.displayStatus();
}

// ═══════════════════════════════════════════════════════════
//  handleInput — one step per key-press edge (no repeat)
// ═══════════════════════════════════════════════════════════
void SpadeGame::handleInput()
{
    static bool upHeld    = false;
    static bool downHeld  = false;
    static bool leftHeld  = false;
    static bool rightHeld = false;

    bool up    = sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
              || sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    bool down  = sf::Keyboard::isKeyPressed(sf::Keyboard::Down)
              || sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    bool left  = sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
              || sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
              || sf::Keyboard::isKeyPressed(sf::Keyboard::D);

    if (up    && !upHeld
        && !maze[playerCol][playerRow].wallTop
        && playerRow > 0)
        --playerRow;

    if (down  && !downHeld
        && !maze[playerCol][playerRow].wallBottom
        && playerRow < rows - 1)
        ++playerRow;

    if (left  && !leftHeld
        && !maze[playerCol][playerRow].wallLeft
        && playerCol > 0)
        --playerCol;

    if (right && !rightHeld
        && !maze[playerCol][playerRow].wallRight
        && playerCol < cols - 1)
        ++playerCol;

    upHeld    = up;
    downHeld  = down;
    leftHeld  = left;
    rightHeld = right;
}

// ═══════════════════════════════════════════════════════════
//  render
// ═══════════════════════════════════════════════════════════
void SpadeGame::render(Player& player)
{
    window.clear(sf::Color(8, 4, 4));
    drawMaze();
    drawExit();
    drawPlayer();
    drawHUD(player);
    if (gameOver) drawEndScreen(won);
    window.display();
}

// ─────────────────────────────────────────────
//  drawMaze
// ─────────────────────────────────────────────
void SpadeGame::drawMaze()
{
    int   offsetX = (800 - cols * cellSize) / 2;
    int   offsetY = 60;
    float wt      = 2.f;
    sf::Color wallCol(180, 20, 20);

    for (int c = 0; c < cols; ++c)
    {
        for (int r = 0; r < rows; ++r)
        {
            float x = static_cast<float>(offsetX + c * cellSize);
            float y = static_cast<float>(offsetY + r * cellSize);

            // Floor
            sf::RectangleShape floor(sf::Vector2f(
                static_cast<float>(cellSize - 1),
                static_cast<float>(cellSize - 1)));
            floor.setPosition(x, y);
            int shade = (c + r) % 2 == 0 ? 22 : 18;
            floor.setFillColor(sf::Color(shade, shade - 2, shade - 2));
            window.draw(floor);

            // Walls
            if (maze[c][r].wallTop)
            {
                sf::RectangleShape w(sf::Vector2f(cellSize + wt, wt));
                w.setPosition(x, y);
                w.setFillColor(wallCol);
                window.draw(w);
            }
            if (maze[c][r].wallLeft)
            {
                sf::RectangleShape w(sf::Vector2f(wt, cellSize + wt));
                w.setPosition(x, y);
                w.setFillColor(wallCol);
                window.draw(w);
            }
            if (c == cols - 1 && maze[c][r].wallRight)
            {
                sf::RectangleShape w(sf::Vector2f(wt, cellSize + wt));
                w.setPosition(x + cellSize, y);
                w.setFillColor(wallCol);
                window.draw(w);
            }
            if (r == rows - 1 && maze[c][r].wallBottom)
            {
                sf::RectangleShape w(sf::Vector2f(cellSize + wt, wt));
                w.setPosition(x, y + cellSize);
                w.setFillColor(wallCol);
                window.draw(w);
            }
        }
    }
}

// ─────────────────────────────────────────────
//  drawPlayer
// ─────────────────────────────────────────────
void SpadeGame::drawPlayer()
{
    int   offsetX = (800 - cols * cellSize) / 2;
    int   offsetY = 60;
    float cx = offsetX + playerCol * cellSize + cellSize / 2.f;
    float cy = offsetY + playerRow * cellSize + cellSize / 2.f;
    float r  = cellSize * 0.30f;

    sf::CircleShape glow(r * 2.f);
    glow.setOrigin(r * 2.f, r * 2.f);
    glow.setPosition(cx, cy);
    glow.setFillColor(sf::Color(0, 200, 80, 40));
    window.draw(glow);

    sf::CircleShape body(r);
    body.setOrigin(r, r);
    body.setPosition(cx, cy);
    body.setFillColor(sf::Color(30, 220, 100));
    body.setOutlineColor(sf::Color(100, 255, 150));
    body.setOutlineThickness(1.5f);
    window.draw(body);
}

// ─────────────────────────────────────────────
//  drawExit — pulsing gold tile
// ─────────────────────────────────────────────
void SpadeGame::drawExit()
{
    int   offsetX = (800 - cols * cellSize) / 2;
    int   offsetY = 60;
    float ex = static_cast<float>(offsetX + (cols - 1) * cellSize);
    float ey = static_cast<float>(offsetY + (rows - 1) * cellSize);

    // Pulse using ctime clock and cmath sin
    float t     = static_cast<float>(std::clock()) / CLOCKS_PER_SEC;
    int   pulse = static_cast<int>(128.f + 127.f * std::sin(t * 4.f));

    sf::RectangleShape exitTile(sf::Vector2f(
        static_cast<float>(cellSize - 2),
        static_cast<float>(cellSize - 2)));
    exitTile.setPosition(ex + 1.f, ey + 1.f);
    exitTile.setFillColor(sf::Color(pulse, pulse / 2, 0, 200));
    exitTile.setOutlineColor(sf::Color(255, 200, 0));
    exitTile.setOutlineThickness(2.f);
    window.draw(exitTile);

    if (fontLoaded && cellSize >= 24)
    {
        sf::Text label("EXIT", font,
                       static_cast<unsigned int>(cellSize / 4));
        label.setFillColor(sf::Color::White);
        label.setPosition(ex + 4.f, ey + cellSize / 3.f);
        window.draw(label);
    }
}

// ─────────────────────────────────────────────
//  drawHUD — uses intToStr (no sstream/sprintf)
// ─────────────────────────────────────────────
void SpadeGame::drawHUD(Player& player)
{
    sf::RectangleShape bar(sf::Vector2f(800.f, 55.f));
    bar.setFillColor(sf::Color(10, 0, 0, 220));
    window.draw(bar);

    if (!fontLoaded) return;

    // Build "TIME: XX.Xs" using cmath floor + intToStr
    char timeBuf[32];
    char secBuf[8];
    int  secs = static_cast<int>(std::floor(timeLeft));
    intToStr(secs, secBuf);
    std::strcpy(timeBuf, "TIME: ");
    std::strcat(timeBuf, secBuf);
    std::strcat(timeBuf, "s");

    sf::Color timeCol = timeLeft > 15.f
                        ? sf::Color(220, 180,  0)
                        : sf::Color(255,  40, 40);
    sf::Text timeText(timeBuf, font, 20);
    timeText.setFillColor(timeCol);
    timeText.setPosition(14.f, 14.f);
    window.draw(timeText);

    // HP
    char hpBuf[32];
    char hpNum[8];
    intToStr(player.getHealth(), hpNum);
    std::strcpy(hpBuf, "HP: ");
    std::strcat(hpBuf, hpNum);

    sf::Text hpText(hpBuf, font, 20);
    hpText.setFillColor(sf::Color(80, 220, 100));
    hpText.setPosition(200.f, 14.f);
    window.draw(hpText);

    // Difficulty
    char diffBuf[32];
    char diffNum[8];
    intToStr(difficulty, diffNum);
    std::strcpy(diffBuf, "SPADE ");
    std::strcat(diffBuf, diffNum);

    sf::Text diffText(diffBuf, font, 20);
    diffText.setFillColor(sf::Color(200, 30, 30));
    diffText.setPosition(380.f, 14.f);
    window.draw(diffText);

    // Hint
    sf::Text hint("ARROWS/WASD to move  |  Reach the EXIT",
                  font, 13);
    hint.setFillColor(sf::Color(120, 120, 120));
    hint.setPosition(14.f, 38.f);
    window.draw(hint);
}

// ─────────────────────────────────────────────
//  drawEndScreen
// ─────────────────────────────────────────────
void SpadeGame::drawEndScreen(bool playerWon)
{
    sf::RectangleShape overlay(sf::Vector2f(800.f, 700.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 170));
    window.draw(overlay);

    if (!fontLoaded) return;

    if (playerWon)
    {
        sf::Text msg("CHALLENGE CLEARED!", font, 46);
        msg.setFillColor(sf::Color(255, 200, 0));
        msg.setPosition(
            800.f / 2.f - msg.getLocalBounds().width / 2.f, 250.f);
        window.draw(msg);

        sf::Text sub("You escaped the maze. You survive... for now.",
                     font, 20);
        sub.setFillColor(sf::Color(200, 200, 200));
        sub.setPosition(
            800.f / 2.f - sub.getLocalBounds().width / 2.f, 320.f);
        window.draw(sub);
    }
    else
    {
        sf::Text msg("GAME OVER", font, 60);
        msg.setFillColor(sf::Color(220, 20, 20));
        msg.setPosition(
            800.f / 2.f - msg.getLocalBounds().width / 2.f, 240.f);
        window.draw(msg);

        sf::Text sub("Time ran out. The Borderland claims you.",
                     font, 20);
        sub.setFillColor(sf::Color(180, 180, 180));
        sub.setPosition(
            800.f / 2.f - sub.getLocalBounds().width / 2.f, 325.f);
        window.draw(sub);
    }

    sf::Text enter("Press ENTER to exit", font, 16);
    enter.setFillColor(sf::Color(120, 120, 120));
    enter.setPosition(
        800.f / 2.f - enter.getLocalBounds().width / 2.f, 400.f);
    window.draw(enter);
}

// ═══════════════════════════════════════════════════════════
//  displayInfo
// ═══════════════════════════════════════════════════════════
void SpadeGame::displayInfo() const
{
    char colBuf[8], rowBuf[8], timeBuf[8], diffBuf[8];
    intToStr(cols,      colBuf);
    intToStr(rows,      rowBuf);
    intToStr(static_cast<int>(timeLimit), timeBuf);
    intToStr(difficulty, diffBuf);

    std::cout << "--- SpadeGame (Maze) Info ---\n";
    std::cout << "Suit       : " << suit     << "\n";
    std::cout << "Difficulty : " << diffBuf  << "\n";
    std::cout << "Maze Size  : " << colBuf
              << "x"             << rowBuf   << "\n";
    std::cout << "Time Limit : " << timeBuf  << "s\n";
    std::cout << "-----------------------------\n";
}
