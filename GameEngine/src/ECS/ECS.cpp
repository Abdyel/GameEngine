#include "ECS.h"
#include "../Logger/Logger.h"
#include <string>

//TODO: implement ECS classes
//initialize nextId int, within IComopnent
int IComponent::nextId = 0;

////////////////////////////////////////////////////////////////////////////////
//Entity implementations
////////////////////////////////////////////////////////////////////////////////
int Entity::GetId() const {
	return id; //encapsulation of member id of entity class.
}

////////////////////////////////////////////////////////////////////////////////
//System Implementations
////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
//Registry Implementations
////////////////////////////////////////////////////////////////////////////////
 
//inserts 
Entity Registry::CreateEntity() {
	int entityId;

	entityId = numEntities++;//incremenet number of entities

	//create new entity with new id and add enitiy to vector to be added.
	Entity entity(entityId);
	entitiesToBeAdded.insert(entity);

	Logger::Log("Entity Created with id = " + std::to_string(entityId));

	return entity;
}

void Registry::Update() {
	//TODO: Add the entities that are waiting to be created to the active Systems
	//TODO: Remove the entities that are waiting to be killed from active systems
}
