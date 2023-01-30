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

class Entity {
	public:
		Entity(int id) : id(id)//using initializer list to initialized id
		{

		};
		int GetId() const;//getter method.
	private:
		int id;
};

//base structure for component class.
struct BaseComponent {
	protected:
		static int nextId;
};

template <typename T>
class Component : BaseComponent {
	
	int id;
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
		Signature& GetComponentSignature() const; //returns reference of a Signature

		//Define the component type T that entities must have to be considered by the system
		//generic type, used for any type of component
		template <typename T> void RequireComponent();

private:
		Signature componentSignature;//which compents an entity must have for the sistem to consider the enitity
		std::vector<Entity> entities;//List of all entities that the system is interested in
};

class Registy {

};

template <typename T>
void System::RequireComponent() {
	const auto componentId = Component<T>::GetId();
	componentSignature.set(componenetID);
}

#endif