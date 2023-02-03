#ifndef ECS_H
#define ECS_H
#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeIndex>
#include <set>
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
class Component : IComponent {
	// Returns the unique id of the component<T>
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
		std::vector<IPool*> componentPools;

		//Vector of component signatures.
		//The signature identifies which components are applied to an entity
		//(vector index = entity id)
		std::vector<Signature> entityComponenetSignatures;

		//map of active systems, index = system typeid
		std::unordered_map<std::type_index, System*> systems;


	public:
		Registry() = default;
		
		void Update();

		Entity CreateEntity();

		template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);

		//void AddEntityToSystem(Entity entitiy);
		//
		// TODO:
		// 
		//
		// GetComponent(Entity entity)
		//  
		// AddSystem()
		//

};

template <typename TComponent, typename ...TArgs> 
void Registry::AddComponent(Entity entity, TArgs&& ...args){
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
		Pool<TComponent>* newComponentPool = new Pool<TComponent>();
		componentPools[componentId] = newComponentPool;
	}

	//get componet pool location that we will be attaching to an entityid to and the new component too
	Pool<TComponent>* componentPool = componentPools[componentId];

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
}


#endif