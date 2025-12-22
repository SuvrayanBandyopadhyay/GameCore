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
	virtual void clear(int entity) = 0;
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
	void clear(int entity) override
	{
		set.clear(entity);
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
	std::unordered_map<std::type_index, std::unique_ptr<ISparseContainer>>components;
	std::unordered_map<std::type_index, std::unique_ptr<IField>>fields;
	//List of free id for reusability
	std::stack<unsigned int> free_ids;
	//The next id to assign
	unsigned int next_id=0;

	//A helper funciton which return the component
	template<typename T>
	SparseSet<T>* _getComponent() 
	{
		std::type_index id = typeid(T);
		auto it = components.find(id);
		if (it == components.end()) 
		{
			it = components.emplace(id, std::make_unique<SparseContainer<T>>()).first;
		}
		return &(static_cast<SparseContainer<T>*>(it->second.get()))->set;

	}
	//A helper function to return fields
	template<typename T>
	Field<T>* _getField() 
	{
		std::type_index id = typeid(T);
		auto it = fields.find(id);
		if (it == fields.end())
		{
			it = fields.emplace(id, std::make_unique<Field<T>>()).first;
		}
		return static_cast<Field<T>*>(it->second.get());
	}
public:
	template<typename T>
	SparseSet<T>& getComponent();
	template<typename T>
	void update(unsigned int ent, T data);
	template<typename T>
	void clear(unsigned int ent);
	template<typename T>
	T& get(int ent);
	template<typename T>
	T& getField();
	template<typename T>
	void setField(T data);
	unsigned int createEntity();
	void erase(unsigned int ent);
	
};

///<summary>
///Returns a sparse set reference to the component
///</summary>
template<typename T>
SparseSet<T>& World::getComponent()
{
	//Static caching
	SparseSet<T>* result = (_getComponent<T>());
	return *result;
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
///Returns a reference to a field
///</summary>
template<typename T>
T& World::getField()
{
	T* result = &(_getField<T>()->data);
	return *result;
}

///<summary>
///Sets the value of a field
///</summary>
template<typename T>
void World::setField(T data)
{
	(_getField<T>()->data) = data;
}


