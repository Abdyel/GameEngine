#ifndef GAME_H
#define GAME_H

#include <SDL.h>

class Game {
	public:
		Game();
		~Game();
		void Initialize(int window_width = 0, int window_height = 0);
		void Run();
		void ProcessInput();
		void Update();
		void Render();
		void Destroy();

		int windowWidth;
		int windowHeight;

	private:
		bool isRunning;
		SDL_Window* window;
		SDL_Renderer* renderer;
};

#endif