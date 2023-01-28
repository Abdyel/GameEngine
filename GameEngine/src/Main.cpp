#include <iostream>
#include "Game.h"
int main(int argc, char* argv[]) {
    //TODO: start game loop
    Game game;

    game.Initialize();
    game.Run();
    game.Destroy();

    return 0;
}