#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Logger/Logger.h"
#include <string>

class CollisionSystem : public System {
public:
	CollisionSystem() {
		RequireComponent<BoxColliderComponent>();
		RequireComponent<TransformComponent>();
	}

	void Update() {
		auto entities = GetSystemEntities();
		
		//loop all entities in required entites 
		for (auto i = entities.begin(); i != entities.end(); i++)
		{
			//dereferenced entity at position i
			Entity a = *i;
			auto aTransform = a.GetComponent<TransformComponent>();
			auto aCollider = a.GetComponent<BoxColliderComponent>();
			//loop only entities that have not been checked yet.
			for (auto j = i; j != entities.end(); j++) {
				Entity b = *j;
				//bypass check if entity is the same as itself.
				if (a == b)
				{
					continue;
				}
				auto bTransform = b.GetComponent<TransformComponent>();
				auto bCollider = b.GetComponent<BoxColliderComponent>();

				bool collisionHappended = checkAABBCollision(
					aTransform.position.x + aCollider.offset.x,
					aTransform.position.y + aCollider.offset.y,
					aCollider.width,
					aCollider.height,
					bTransform.position.x + bCollider.offset.x,
					bTransform.position.y + bCollider.offset.y,
					bCollider.width,
					bCollider.height
					);
				if (collisionHappended) {
					Logger::Log("Entity [" + std::to_string(a.GetId()) + "] and Entity [" + std::to_string(b.GetId()) + "] have collided");
					
					// TODO: emit an event..
				}
			}
		}
	}

	bool checkAABBCollision(double aX, double aY, double aW, double aH, double bX, double bY, double bW, double bH)
	{
		return (
			aX < bX + bW &&
			aX + aW > bX &&
			aY < bY + bH &&
			aY + aH > bY 
		);
	}
};
#endif
