#include "Game.h"
#include "../Logger/Logger.h"
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <iostream>

Game::Game() {
	isRunning = false;
	windowWidth = 0;
	windowHeight = 0;
	window = NULL;//initializing window as null
	renderer = NULL;//initializing renderer as null
	Logger::Log("game constructor called");
}
Game::~Game() {
	Logger::Log("game destructor called");
}
//Game Initialize is used for setting up SDL window and SDL renderer
void Game::Initialize(int window_width, int window_height) {
	//Checks to make sure sdl can initialize proscesses
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		Logger::Err("ERROR: unable to initialize SDL.");
		return;
	}

	//If user chooses to initialize game at custom window resolution/size do not use full monitor resolution
	if (window_width != 0 && window_height != 0)
	{
		windowWidth = window_width;
		windowHeight = window_height;
	}
	else
	{	
		//sets window to resolution of users screen size.
		SDL_DisplayMode displayMode;
		SDL_GetCurrentDisplayMode(0, &displayMode);
		windowWidth = displayMode.w;
		windowHeight = displayMode.h;
	}

	//raw pointer to a struct. points to address where struct is located
	//creates a window on operating system
	//window is a private memeber to the game class in game.h file
	window = SDL_CreateWindow(
		"Game Engine",//title
		SDL_WINDOWPOS_CENTERED,//x position
		SDL_WINDOWPOS_CENTERED,// y position
		windowWidth,//width
		windowHeight,//height
		SDL_WINDOW_BORDERLESS //Creates borderless window
		);

	//Checks to make sure window was able to be created
	if (!window)
	{
		Logger::Err("ERROR: could not create SDL window.");
		return;
	}

	//Creates renderer that will belong to SDL window, using default monitor,
	//renderer is a private member to the game class in game.h file
	//Ending flags tell SDL to use hardware acceleration and also use VSync to prevent any screen tearing.
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	//Checks to make sure window was able to be created
	if (!renderer)
	{
		Logger::Err("ERROR: could not create SDL renderer.");
		return;
	}

	//if game window is initialized with non defualt values set a windowed fullscreen mode of program
	if (window_width != 0 && window_height != 0)
	{
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	}

	//sets is running variable to tru so long as all sdl process are initialized
	isRunning = true;
}
//Controls time the run function preforms game loop. not to be controled by how fast processor is.
void Game::timeControl() {
	//calculates time to wait for 
	int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
	
	//checks timeToWait to make sure it is a valid value.S 
	if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME)
	{
		SDL_Delay(timeToWait);
	}

	//difference in ticks from last frame. converted to second
	//this creates a pixels per second relationship within update method.
	//use this time for any moving game object by multiplying it with deltatime.
	deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

	//store frame time
	millisecsPreviousFrame = SDL_GetTicks();
}

//JUST FOR TESTING
glm::vec2 playerPosition;
glm::vec2 playerVelocity;

void Game::Run() {
	Setup();
	while (isRunning) {
		ProcessInput();
		Update();
		Render();
		timeControl();
	}
}

//method used to setup game object location, size, etc...
void Game::Setup() {
	//setup game object
	playerPosition = glm::vec2(10.0,20.0);
	playerVelocity = glm::vec2(500.0,0.0);
}

void Game::Update() {
	playerPosition.x += playerVelocity.x * deltaTime;
	playerPosition.y += playerVelocity.y * deltaTime;
}

void Game::Render() {
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);//draws background of window
	SDL_RenderClear(renderer);//sets background to the above color of renderer.

	//draw a PNG texture
	SDL_Surface* surface = IMG_Load("assets/images/tank-tiger-right.png");//creates surface of a png image
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);//creates texture from the surface that can be rendered
	SDL_FreeSurface(surface);//clears surface and frees it from memory

	//creates SDL rectangle that will bind the size and location of a rendered texture
	SDL_Rect dstRect = 
	{ 
		static_cast<int>(playerPosition.x), //x location is decided by player position vector
		static_cast<int>(playerPosition.y), //y location is decided by player position vector
		32, 
		32
	};
	//renderer copys information from texture, (renderer, texture, portion of text that will be used, address SDL_primitve for location and size of texture)
	SDL_RenderCopy(renderer, texture, NULL, &dstRect);//NULL uses for size of texture
	SDL_DestroyTexture(texture);//frees memory of texture


	SDL_RenderPresent(renderer);//presents what is on renderer to window
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

void Game::Destroy() {
	//Free memory used by sdl renderer and window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}