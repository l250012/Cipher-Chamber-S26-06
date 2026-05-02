#include <iostream>
#include "Player.h"
#include "Card.h"
#include "SpadeGame.h"

int main()
{
    std::cout << "========================================\n";
    std::cout << "  ALICE IN BORDERLAND - SPADE MAZE\n";
    std::cout << "========================================\n\n";

    Player player("Arisu", 100, 80, 100);
    player.displayStatus();

    // Difficulty 1-13 — change to make maze bigger/harder
    Card* challenge = new SpadeGame(3);

    static_cast<SpadeGame*>(challenge)->displayInfo();

    std::cout << "\nSFML window opening...\n";
    std::cout << "ARROW KEYS or WASD to move.\n";
    std::cout << "Reach the gold EXIT before time runs out!\n\n";

    challenge->startChallenge(player);

    std::cout << "\n========================================\n";
    std::cout << "  CHALLENGE ENDED\n";
    std::cout << "========================================\n";

    delete challenge;
    return 0;
}
