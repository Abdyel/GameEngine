#include "ECS.h"

//TODO: implement ECS classes

int Entity::GetId() const {
	return id; //encapsulation of member id of entity class.
}



void System::AddEntityToSystem(Entity entity) {
	entities.push_back(entity);
}

//iterates through entities in system then deletes entities that matches given entity
void System::RemoveEntitFromSystem(Entity entity) {
	entities.erase(std::remove_if(entities.begin(), entities.end(), 
		[&entity](Entity other) {
			return entity == other;//creates lambda/anonymous function
		}), entities.end());
}

std::vector<Entity> System::GetSystemEntities() const {
	return entities;
}

const Signature& System::GetComponentSignature() const {
	return componentSignature;
}

