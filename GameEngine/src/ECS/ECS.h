#ifndef ECS_H
#define ECS_H
#include <bitset>
#include <vector>

/////////////////////////////////////////////////////////////////////////////
//create signature datatype using bitset to track which components an enitiy has
//and keeps track of which enities a system is interested in.
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

	private:
		int id;
};
/////////////////////////////////////////////////////////////////////////////
// C O M P O N E N T
// //////////////////////////////////////////////////////////////////////////
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
		T& Get(int index)		{ return static_cast<T&> data[index]; }

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
		int numEntities = 0;
		//Vector of component pools, each pool contains all the data for each type of component
		//vector index is the component type id.
		std::vector<IPool*> componentPools;
};


#endif