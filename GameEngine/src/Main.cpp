#include <iostream>
#include "Game/Game.h"
int main(int argc, char* argv[]) {
    //TODO: start game loop
    Game game;

    game.Initialize(1920, 1080);
    game.Run();
    game.Destroy();

    return 0;
}