#pragma once
#include<iostream>
#include<queue>
#include<any>
#include<unordered_map>
#include<typeinfo>
#include<typeindex>
#include <stdexcept>
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
	//The registry mapping size_t to a container
	std::unordered_map<size_t, std::unique_ptr<ISparseContainer>>components;
	std::unordered_map<size_t, std::unique_ptr<IField>>fields;
	//A helper funciton which return the component
	template<typename T>
	SparseSet<T>* _getComponent() 
	{
		size_t id = typeid(T).hash_code();
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
		size_t id = typeid(T).hash_code();
		auto it = fields.find(id);
		if (it == fields.end())
		{
			it = fields.emplace(id, std::make_unique<Field<T>>()).first;
		}
		return static_cast<Field<T>*>(it->second.get());
	}
public:
	//Get Component
	template<typename T>
	SparseSet<T>& getComponent() 
	{
		//Static caching
		static SparseSet<T>* result = (_getComponent<T>());
		return *result;
	}
	//Update Component
	template<typename T>
	void update(unsigned int ent, T data) 
	{
		auto& set =  getComponent<T>();
		set.update(ent, data);
	}
	//Clear Component
	template<typename T>
	void clear(unsigned int ent) 
	{
		auto& set = getComponent<T>();
		set.clear(ent);
	}
	//Get entity value
	template<typename T>
	T& get(int ent)
	{
		auto& set = getComponent<T>();
		return *set.get(ent);
	}
	//Get field value
	template<typename T>
	T& getField()
	{
		static T* result = &(_getField<T>()->data);
		return *result;
	}
	//Set field value
	template<typename T>
	void setField(T data)
	{
		(_getField<T>()->data) = data;
	}
};
