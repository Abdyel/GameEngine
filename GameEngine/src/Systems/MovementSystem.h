#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include <string>

//movement system inherits from System class
class MovementSystem : public System {
	public:
		MovementSystem() {
			RequireComponent<TransformComponent>();
			RequireComponent<RigidBodyComponent>();
		}
		
		void Update(double deltaTime) 
		{
			//Loop all entites that the system is interested in
			for (auto entity : GetSystemEntities())
			{
				//you could use auto& to make it smaller. left it as reference transformcomponent to see what is happening better.
				TransformComponent& transform = entity.GetComponent<TransformComponent>();
				const RigidBodyComponent rigidbody = entity.GetComponent<RigidBodyComponent>();

				transform.position.x += rigidbody.velocity.x * deltaTime;
				transform.position.y += rigidbody.velocity.y * deltaTime;

				Logger::Log("Entity ID: " + std::to_string(entity.GetId()) + " position is now (" + std::to_string(transform.position.x) +" ,"+ std::to_string(transform.position.x) + " )");
			}
		}
};


#endif
