#ifndef GAME_H
#define GAME_H

#include <SDL.h>

const int FPS = 30;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game {
	//public methods are the public api. application programming interface.
	public:
		Game();
		~Game();
		void Initialize(int window_width = 0, int window_height = 0);
		void Run();
		void Setup();
		void ProcessInput();
		void Update();
		void Render();
		void timeControl();
		void Destroy();

		int windowWidth;
		int windowHeight;

	private:
		bool isRunning;
		double deltaTime = 0;
		int millisecsPreviousFrame = 0;
		SDL_Window* window;
		SDL_Renderer* renderer;
};

#endif