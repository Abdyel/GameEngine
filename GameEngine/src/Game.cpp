#include "Game.h"
#include <SDL.h>
#include <iostream>

Game::Game() {
	window = NULL;//initializing window as null
	renderer = NULL;//initializing renderer as null
	isRunning = true;//isRunning is private member of class of type bool. 
	std::cout << "game constructor called" << std::endl;
}
Game::~Game() {
	std::cout << "game destructor called" << std::endl;
}
void Game::Initialize() {
	//Checks to make sure sdl can initialize proscesses
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cerr << "ERROR: unable to initialize SDL." << std::endl;
		return;
	}

	//raw pointer to a struct. points to address where struct is located
	//creates a window on operating system
	//window is a private memeber to the game class in game.h file
	window = SDL_CreateWindow(
		"Game Engine",//title
		SDL_WINDOWPOS_CENTERED,//x position
		SDL_WINDOWPOS_CENTERED,// y position
		800,//width
		600,//height
		SDL_WINDOW_BORDERLESS //Creates borderless window
		);

	//Checks to make sure window was able to be created
	if (!window)
	{
		std::cerr << "ERROR: could not create SDL window." << std::endl;
		return;
	}

	//Creates renderer that will belong to SDL window, using default monitor,
	//renderer is a private member to the game class in game.h file
	renderer = SDL_CreateRenderer(window, -1, 0);

	//Checks to make sure window was able to be created
	if (!renderer)
	{
		std::cerr << "ERROR: could not create SDL renderer." << std::endl;
		return;
	}

}
void Game::Run() {
	while (isRunning) {
		ProcessInput();
		Update();
		Render();
	}
}
void Game::ProcessInput() {
	//while there is an event inside sdlEvent poll event returns true until all event are  read.
	SDL_Event sdlEvent;//not a pointer. this creates structure in memory
	while (SDL_PollEvent(&sdlEvent)) //passing reference of the struct
	{
		switch (sdlEvent.type)
		{
			//sdl quit is pressing x on window.
			case SDL_QUIT:
				isRunning = false;//changes is running to false which will stop game loop
				break;
			case SDL_KEYDOWN:
				if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
					isRunning = false;
				}
		}

	}
}
void Game::Update() {

}
void Game::Render() {
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);//draws background of window
	SDL_RenderClear(renderer);//clears renderer screen

	//TODO: Render all game objects.

	SDL_RenderPresent(renderer);//presents what is on renderer to window
}
void Game::Destroy() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}