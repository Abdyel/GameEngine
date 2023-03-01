#pragma once

#include "../Logger/Logger.h"

class EventBus {
public:
	EventBus() {
		Logger::Log("EventBus Constructor called.");
	}
	~EventBus() {
		Logger::Log("EventBus Destructor Called.");

	}
	
	////////////////////////////////////////////////////////
	// Subscribe to an event type <T>
	// a listender subscribes to an event
	// Example: eventBus->SubscribeToEvent<CollisionEVent>(&Game::onCollision);
	////////////////////////////////////////////////////////
	void SubscribeToEvent<>(){
		//TODO:
	}
	////////////////////////////////////////////////////////
	// Emit an event type <T>
	// as soon as something emits an event, 
	// execute all the listen callback functions
	// Example: eventBus->EmitEvent<CollisionEvent>(player, enemy);
	////////////////////////////////////////////////////////
	void EmitEvent<>() {
		//TODO:
	}
};