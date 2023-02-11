#include "AssetStore.h"	
#include "../Logger/Logger.h"
#include <SDL_image.h>

AssetStore::AssetStore() {
	Logger::Log("Asset Store Constructor Called.");
}

AssetStore::~AssetStore() {
	ClearAssets();
	Logger::Log("Asset Store Destuctor Called");
}

void AssetStore::ClearAssets() {
	for (auto texture : textures) {
		SDL_DestroyTexture(texture.second);
	}
	textures.clear();
	Logger::Log("Assets Cleared from store.");
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath) {
	SDL_Surface* surface = IMG_Load(filePath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	//add the texture to the map
	textures.emplace(assetId, texture);

	Logger::Log("Asset [" + assetId + "] added to Asset store");
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetId) {
	return textures[assetId];
}