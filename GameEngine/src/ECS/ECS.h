#ifndef ECS_H
#define ECS_H
#include "../Logger/Logger.h"
#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeIndex>
#include <set>
#include <memory>
#include <string>
/////////////////////////////////////////////////////////////////////////////
// S I G N A T U R E
/////////////////////////////////////////////////////////////////////////////
//create signature datatype using bitset to track which components an enitiy has
//and keeps track of which enities a system is interested in.
/////////////////////////////////////////////////////////////////////////////
const unsigned int MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> Signature;
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/// E N T I T Y
/////////////////////////////////////////////////////////////////////////////
/// entities are objects that only give an identifier to a game object
/////////////////////////////////////////////////////////////////////////////
class Entity {
	public:
		Entity(int id) : id(id) {}; //using initializer list to initialized id
		Entity(const Entity& entity) = default; //overloaded constructor using default build.
		int GetId() const;//getter method.

		//operator overloading to define the meaning of creating an entities based on another
		Entity& operator = (const Entity& other) = default;
		//operator overloading to define the meaning of comparing two entities with ==
		bool operator == (const Entity& other) const {return id == other.id;}
		//operator overloaded to define the meaning of comparing two entities with !=
		bool operator != (const Entity& other) const {return id != other.id;}
		bool operator >(const Entity& other) const { return id > other.id; }
		bool operator <(const Entity& other) const { return id < other.id; }

	private:
		int id;
};
/////////////////////////////////////////////////////////////////////////////
// C O M P O N E N T
/////////////////////////////////////////////////////////////////////////////
//base structure for component class. Interface Component.
struct IComponent {
	protected://only usuable to inherited classes or part of class
		static int nextId;
};
//used to assign a unique ids to a component type. generic TComponent
template <typename TComponent>
class Component: public IComponent {
	// Returns the unique id of the component<T>
	public:
	static int GetId() {
		static auto id = nextId++;
		return id;
	}
};
	
///////////////////////////////////////////////////////////////////////////
//  S Y S T E M
///////////////////////////////////////////////////////////////////////////
// The system processes entities that contain a specific signature
///////////////////////////////////////////////////////////////////////////

class System {
	public:
		System() = default;
		virtual ~System() = default;

		void AddEntityToSystem(Entity entity);
		void RemoveEntitFromSystem(Entity entity);
		std::vector<Entity> GetSystemEntities() const;
		const Signature& GetComponentSignature() const; //returns reference of a Signature

		//Define the component type T that entities must have to be considered by the system
		//generic type, used for any type of component
		template <typename T> void RequireComponent();

	private:
		Signature componentSignature;//which compents an entity must have for the system to consider the enitity
		std::vector<Entity> entities;//List of all entities that the system is interested in

};

//Implements the require component to decide what components matter to a given system
template <typename TComponent>
void System::RequireComponent() {
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}

//////////////////////////////////////////////////////////////////////////
// P O O L
//////////////////////////////////////////////////////////////////////////
// A pool is just a vector {contigous data} of objects of type T
// Pool is a wrapper for a vector of different types of components
/////////////////////////////////////////////////////////////////////////

class IPool {
	public:
		virtual ~IPool() = 0 {} //purely vitural type. Ipool used as interface to allow use in registry
};

template <typename T>
class Pool : public IPool
{
	private:
		std::vector<T> data;
	public:
		Pool(int size = 100)	{ data.resize(size); }
		~Pool() = default;
		bool isEmpty() const	{ return data.empty(); }
		int GetSize() const		{ return data.size(); }
		void Resize(int n)		{ data.resize(n); }
		void Clear()			{ data.clear(); }
		void Add(T object)		{ data.push_back(object); }
		void Set(int index, T object) { data[index] = object; }
		T& Get(int index)		{ return static_cast<T&>(data[index]); }

		T& operator [](unsigned int index) {
			return data[index];
		}
};

//////////////////////////////////////////////////////////////////////////
/// R E G I S T R Y
//////////////////////////////////////////////////////////////////////////
/// The registry manages the creation and destruction of entities. as well
/// as adding systems and adding components to entities
//////////////////////////////////////////////////////////////////////////

class Registry {
	private:
		//management of entities
		int numEntities = 0;

		//Sets of entites that are flagged to be added or removed. in next regisrty update.
		std::set<Entity> entitiesToBeAdded;
		std::set<Entity> entitiesToBeKilled;

		//Vector of component pools, each pool contains all the data for each type of component
		//vector index is the component type id.
		//pool index is the entity id.
		std::vector<std::shared_ptr<IPool>> componentPools;

		//Vector of component signatures.
		//The signature identifies which components are applied to an entity
		//(vector index = entity id)
		std::vector<Signature> entityComponenetSignatures;

		//map of active systems, index = system typeid
		std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

	public:
		//prototype registry constructor 
		Registry() { 
			Logger::Log("Registry constructor called"); 
		}
		~Registry(){
			Logger::Log("Registry destructor called");
		}
		//update function that will update all entities, components, and systems in registry
		void Update();
		//method that will create a new entity and add it to entity to be added list
		Entity CreateEntity();

		///// Component related functions /////
		//function that adds a component to an entity
		template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
		//Function that will remove a component from an entity
		template <typename TComponent> void RemoveComponent(Entity entity);
		//checks an entity to see if it has a specific component returns true if it does, false if it doesnt.
		template <typename TComponent> bool HasComponent(Entity entity) const;

		////// System related functions /////
		//adds system to registry unordered map "systems"
		template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
		//removes system from registry map
		template <typename TSystem> void RemoveSystem();
		//returns true or false if system is in regisrty map
		template <typename TSystem> bool HasSystem() const;
		//returns with system value inside of the unordered map "systems"
		template <typename TSystem> TSystem& GetSystem() const;

		// checks the component signature of an entity and add the entity 
		//     to the systems that are interested in it.
		void AddEntityToSystems(Entity entity);
};
//implemented template fuctions from registry prototype
/////// F U N C T I O N S   F O R   C O M P O N E N T S ///////
template <typename TComponent, typename ...TArgs> 
void Registry::AddComponent(Entity entity, TArgs&& ...args) 
{
	//get component id and entity id that we will be attac
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	//if component id is large than the componentPools vector size, resize to fit new component
	if (componentId >= componentPools.size())
	{
		componentPools.resize(componentId + 1, nullptr);
	}
	//if component pool at index does not exist create a new component pool at index
	if (!componentPools[componentId])
	{
		std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
		componentPools[componentId] = newComponentPool;
	}

	//get component pool location that we will be attaching to an entityid to and the new component too
	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

	//if component pool is to small to accomedate the entity increase size of pool
	if (entityId >= componentPool->GetSize()) {
		componentPool->Resize(numEntities);
	}

	//create new component using default constructor
	TComponent newComponent(std::forward<TArgs>(args)...);

	//place the new component created at the entity's indexed position
	//Place new componet into pool with entityid
	componentPool->Set(entityId, newComponent);

	//turn the component signature for the entity as "on" for the given component.
	entityComponenetSignatures[entityId].set(componentId);

	Logger::Log("Component ID [" + std::to_string(componentId) + "] was added to entity ID: " + std::to_string(entityId));
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity) {
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	entityComponenetSignatures[entityId].set(componentId, false);
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const 
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	//test signature within Signature of the index location of entityid. 
	return entityComponenetSignatures[entityId].test(componentId);
}

/////// F U N C T I O N S   F O R   S Y S T E M S ///////
template <typename TSystem, typename ...TArgs> 
void Registry::AddSystem(TArgs&& ...args) {
	std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(TSystem), newSystem)));
}

template <typename TSystem>
void Registry::RemoveSystem() {
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}

template <typename TSystem>
bool Registry::HasSystem() const {
	return (system.find(std::type_index(typeid(TSystem))) != systems.end());
}

template <typename TSystem> 
TSystem& Registry::GetSystem() const{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(system->second));
}
#endif