#pragma once

#include <map>
#include <string>
#include <SDL.h>

class AssetStore {
	private:
		std::map<std::string, SDL_Texture*> textures;
		//TODO: ceate map for fonts
		//TODO: ceate map for audio

	public:
		AssetStore();
		~AssetStore();

		void ClearAssets();

		void AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath);
		
		SDL_Texture* GetTexture(const std::string& assetId);
};