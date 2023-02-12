#include "Game.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/RenderColliderSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>

Game::Game() {
	isRunning = false;
	isDebug = false;
	windowWidth = 0;
	windowHeight = 0;
	window = NULL;//initializing window as null
	renderer = NULL;//initializing renderer as null

	//makes the regitry for ECS and assetStore for textures, audio, and fonts
	registry = std::make_unique<Registry>();
	assetStore = std::make_unique<AssetStore>();

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
	deltaTime = ((SDL_GetTicks() - millisecsPreviousFrame) / 1000.0);

	//store frame time
	millisecsPreviousFrame = SDL_GetTicks();
}
//function that will start the game loop after initiaization of game
void Game::Run() {
	Setup();
	while (isRunning) {
		ProcessInput();
		Update();
		Render();
		timeControl();
	}
}

void Game::LoadScene(int sceneId) {
	// Add the sytems that need to be processed in our game
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();
	registry->AddSystem<AnimationSystem>();
	registry->AddSystem<CollisionSystem>();
	registry->AddSystem<RenderColliderSystem>();

	// Adding assets to the asset store
	assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
	assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");
	assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper.png");
	assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");

	// Load the tilemap
	int tileSize = 32;
	double tileScale = 1.0;
	int mapNumCols = 25;
	int mapNumRows = 20;

	std::fstream mapFile;
	mapFile.open("./assets/tilemaps/jungle.map");

	for (int y = 0; y < mapNumRows; y++) {
		for (int x = 0; x < mapNumCols; x++) {
			char ch;
			mapFile.get(ch);
			int srcRectY = std::atoi(&ch) * tileSize;
			mapFile.get(ch);
			int srcRectX = std::atoi(&ch) * tileSize;
			mapFile.ignore();

			Entity tile = registry->CreateEntity();
			tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);
			tile.AddComponent<SpriteComponent>("tilemap-image",tileSize, tileSize, 0, srcRectX, srcRectY);
		}
	}
	mapFile.close();

	// Create an entity
	Entity chopper = registry->CreateEntity();
	chopper.AddComponent<TransformComponent>(glm::vec2(100.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
	chopper.AddComponent<RigidBodyComponent>(glm::vec2(10.0, 0.0));
	chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 2);
	chopper.AddComponent<AnimationComponent>(2, 10);

	Entity radar = registry->CreateEntity();
	radar.AddComponent<TransformComponent>(glm::vec2(10.0, 400.0), glm::vec2(1.0, 1.0), 0.0);
	radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 1);
	radar.AddComponent<AnimationComponent>(8, 10);


	
	Entity tank = registry->CreateEntity();
	tank.AddComponent<TransformComponent>(glm::vec2(300.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(-20.0, 0.0));
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 1);
	tank.AddComponent<BoxColliderComponent>(32,32);

	Entity truck = registry->CreateEntity();
	truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 0.0));
	truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
	truck.AddComponent<BoxColliderComponent>(32,32);
}

//method used to setup game object location, size, etc...
void Game::Setup() {
	LoadScene(1);
}

void Game::Update() {
	//Update the registry to process the entites that are waiting to be created or destroyed
	registry->Update();
	//System updates positions of entites based on rigidBody values.
	registry->GetSystem<CollisionSystem>().Update();
	registry->GetSystem<MovementSystem>().Update(deltaTime);
	registry->GetSystem<AnimationSystem>().Update(deltaTime);
}

void Game::Render() {
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);//draws background of window
	SDL_RenderClear(renderer);//sets background to the above color of renderer.

	//System renders images to the location based on transform component
	registry->GetSystem<RenderSystem>().Update(renderer, assetStore);
	//Debug rendered items, such as collision boxes.
	if(isDebug){ registry->GetSystem<RenderColliderSystem>().Update(renderer); }


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
			if (sdlEvent.key.keysym.sym == SDLK_p) {
				isDebug = !isDebug;
			}
			break;
		}

	}
}

void Game::Destroy() {
	//Free memory used by sdl renderer and window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}