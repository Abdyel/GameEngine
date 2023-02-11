#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../AssetStore/AssetStore.h"
#include "../ECS/ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include <SDL.h>
#include <SDL_image.h>

class RenderSystem : public System {
	public:
		RenderSystem() {
			RequireComponent<SpriteComponent>();
			RequireComponent<TransformComponent>();
		}

		void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore)
		{
			//Loop all entites that the system is interested in
			for (auto entity : GetSystemEntities())
			{
				const auto transform = entity.GetComponent<TransformComponent>();
				const auto sprite = entity.GetComponent<SpriteComponent>();

				//set source rectangle for out original sprite texture
				SDL_Rect srcRect = sprite.srcRect;

				//set the destination rectangle with the x,y position to be rendered
				SDL_Rect dstRect = {
					static_cast<int>(transform.position.x),
					static_cast<int>(transform.position.y),
					static_cast<int>(sprite.width * transform.scale.x),
					static_cast<int>(sprite.height * transform.scale.y)
				};

				SDL_RenderCopyEx(
					renderer,
					assetStore->GetTexture(sprite.assetId),
					&srcRect,
					&dstRect,
					transform.rotation,
					NULL,//center point where the rotation will happen, and null it is at the center of srcRect
					SDL_FLIP_NONE//controls if the image is fliped
				);
			}
		}
};



#endif
