#include "Game.h"

// Inspiration taken from https://www.raylib.com/examples.html

int main()
{
    Game game(1900, 900);
    game.Initialize();
    game.Run();
    return 0;
}