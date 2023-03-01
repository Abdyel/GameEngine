#pragma once
#include "../ECS/ECS.h"
#include "Event.h"

class CollisionEvent: public Event {
	public:
		Entity a;
		Entity b;
		CollisionEvent(Entity a, Enttiy b) : a(a), b(b) {};
};