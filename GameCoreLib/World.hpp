#pragma once
#include<iostream>
#include<queue>
#include<any>
#include<unordered_map>
#include<typeinfo>
#include<typeindex>
#include <stdexcept>
#include<stack>
#include"SparseSet.hpp"

/*
Sparsets represent an entire array of similar attributes e.g coordinates, speeds,etc.
Fields are single values which dont need a sparset of their own
*/
class ISparseContainer
{
public:
	virtual ~ISparseContainer() = default;
	virtual std::type_index type() = 0;
	virtual void erase(int entity) = 0;
};

template<typename T>
class SparseContainer : public ISparseContainer
{
public:
	SparseSet<T>set;
	std::type_index type() override
	{
		return typeid(T);
	}
	void erase(int entity) override
	{
		set.erase(entity);
	}
};

class IField
{
public:
	virtual ~IField() = default;
	virtual std::type_index type() = 0;
};

template<typename T>
class Field : public IField
{
public:
	T data;
	std::type_index type() override
	{
		return typeid(T);
	}
};



class World
{
private:
	//The registry mapping type_index to a container
	std::vector<std::unique_ptr<ISparseContainer>>components;
	std::vector<std::unique_ptr<IField>>fields;
	//List of free id for reusability
	std::stack<unsigned int> free_ids;
	unsigned int next_component = 0;//Next component to assign
	unsigned int next_field = 0;//Next field to assign
	//Function to get unique ids for each field
	template<typename T>
	unsigned int getFieldID();
	
public:
	//The next id to assign
	unsigned int next_id = 0;
	template<typename T>
	SparseSet<T>& getComponent();
	template<typename T>
	void update(unsigned int ent, T data);
	template<typename T>
	void update(unsigned int ent);
	template<typename T>
	void clear(unsigned int ent);
	template<typename T>
	T& get(int ent);
	template<typename T>
	T& getField();
	template<typename T>
	void setField(T data);
	template<typename T>
	bool fieldExists();
	template<typename T>
	void deleteField();
	unsigned int createEntity();
	void erase(unsigned int ent);
	
};
///<summary>
///Returns a sparse set reference to the component
///</summary>
template<typename T>
SparseSet<T>& World::getComponent()
{
	static unsigned int id = next_component++;
	if (id >= components.size()) 
	{
		components.resize(id + 1);
	}
	if (!components[id]) 
	{
		components[id] = std::make_unique<SparseContainer<T>>();
	}
	return static_cast<SparseContainer<T>*>(components[id].get())->set;
}

///<summary>
///Updates the component of an entity
///</summary>
template<typename T>
void World::update(unsigned int ent, T data)
{
	auto& set = getComponent<T>();
	set.update(ent, data);
}

///<summary>
///Special update for tags
///</summary>
template<typename T>
void World::update(unsigned int ent)
{
	auto& set = getComponent<T>();
	set.update(ent);
}

///<summary>
///Deletes a component of an entity if it exists
///</summary>
template<typename T>
void World::clear(unsigned int ent)
{
	auto& set = getComponent<T>();
	set.clear(ent);
}

///<summary>
///Returns a reference to the component of an entity
///</summary>
template<typename T>
T& World::get(int ent)
{
	auto& set = getComponent<T>();
	return *set.get(ent);
}

///<summary>
///Returns a unique identity for the component
///</summary>
template<typename T>
unsigned int World::getFieldID()
{
	static unsigned int id = next_field++;
	return id;
}

///<summary>
///Checks if a field exists
///</summary>
template<typename T>
bool World::fieldExists()
{
	static unsigned int id = getFieldID<T>();
	if (id>=fields.size()||!fields[id]) 
	{
		return false;
	}
	return true;
}

///<summary>
///Checks if a field exists
///</summary>
template<typename T>
void World::deleteField()
{
	static unsigned int id = getFieldID<T>();
	if (id < fields.size())
	{
		fields[id].reset();
	}
}

///<summary>
///Returns a reference to a field
///</summary>
template<typename T>
T& World::getField()
{
	static unsigned int id = getFieldID<T>();
	if (id >= fields.size())
	{
		fields.resize(id + 1);
	}
	if (!fields[id])
	{
		fields[id] = std::make_unique<Field<T>>();
	}
	return static_cast<Field<T>*>(fields[id].get())->data;
}

///<summary>
///Sets the value of a field
///</summary>
template<typename T>
void World::setField(T data)
{
	(getField<T>()) = data;
}


///<summary>
///Views allow us to easily select entities which have and dont have certain components from a world 
/// </summary>

template<typename... Cs>
class View
{
	World* w;
	std::vector<int>* driver_dense = nullptr;
	std::vector<int*> sparse_maps;
	std::tuple<std::vector<Cs>*...> data_arrays;

public:
	View(World& world) : w(&world)
	{
		(select_driver<Cs>(), ...);
		(cache_component<Cs>(), ...);
	}

	class Iterator
	{
		View* view;
		size_t idx;

	public:
		Iterator(View* v, size_t i) : view(v), idx(i)
		{
			skip_invalid();
		}

		void skip_invalid()
		{
			while (idx < view->driver_dense->size())
			{
				int e = (*view->driver_dense)[idx];
				bool ok = true;

				for (int* sparse : view->sparse_maps)
				{
					if (sparse[e] == -1)
					{
						ok = false;
						break;
					}
				}

				if (ok) break;
				idx++;
			}
		}

		Iterator& operator++()
		{
			idx++;
			skip_invalid();
			return *this;
		}

		bool operator!=(const Iterator& o) const
		{
			return idx != o.idx;
		}

		auto operator*()
		{
			int e = (*view->driver_dense)[idx];
			return view->get_tuple(e);
		}
	};

	Iterator begin() { return Iterator(this, 0); }
	Iterator end() { return Iterator(this, driver_dense->size()); }

private:
	template<typename T>
	void select_driver()
	{
		auto& comp = w->getComponent<T>();
		if (!driver_dense || comp.dense.size() < driver_dense->size())
			driver_dense = &comp.dense;
	}

	template<typename T>
	void cache_component()
	{
		auto& comp = w->getComponent<T>();
		sparse_maps.push_back(comp.sparse.data());
		std::get<std::vector<T>*>(data_arrays) = &comp.data;
	}

	auto get_tuple(int entity)
	{
		return std::tuple<Cs&...>(
			std::get<std::vector<Cs>*>(data_arrays)->at(
				w->getComponent<Cs>().sparse[entity]
			)...
		);
	}
};

