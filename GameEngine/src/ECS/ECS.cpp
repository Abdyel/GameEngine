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
	entity.registry = this;
	entitiesToBeAdded.insert(entity);

	// Make sure the entityComponentSignature vector can accomodate the new entity
	if (entityId >= entityComponenetSignatures.size())
	{
		entityComponenetSignatures.resize(entityId + 1);
	}

	Logger::Log("Entity Created with id = " + std::to_string(entityId));

	return entity;
}

//adds entity to a system that is interested in the entity
void Registry::AddEntityToSystems(Entity entity) {
	const auto entityId = entity.GetId();

	//gives bit set values that control which components are part of an entity
	const auto entityComponentSignature = entityComponenetSignatures[entityId];
	
	// loop all the systems
	for (auto& system: systems) {
		const auto& systemComponentSignature = system.second->GetComponentSignature();
		
		//preform AND bitwise between entity & systen component signature.
		//results in removing any bits that are not included in systemComponentSignature
		//iff the result of AND bitwise comparison matches the system component signature then 
		//logic will return true.
		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

		if (isInterested) 
		{
			// add entity to system
			system.second->AddEntityToSystem(entity);
		}
	}

}

void Registry::Update() {
	//TODO: Add the entities that are waiting to be created to the active Systems
	for (auto entity : entitiesToBeAdded)
	{
		AddEntityToSystems(entity);
	}
	entitiesToBeAdded.clear();

	//TODO: Remove the entities that are waiting to be killed from active systems
	
}


