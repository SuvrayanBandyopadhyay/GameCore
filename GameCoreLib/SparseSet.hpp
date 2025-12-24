#pragma once
#include<iostream>
#include<vector>

template<typename T>
class SparseSet 
{
public:
	std::vector<unsigned int>sparse;
	std::vector<unsigned int>dense;
	std::vector<T>data;
	void addEntity(unsigned int ent, T d);
	bool find(unsigned int ent);
	void remove(unsigned int ent);
	void erase(unsigned int ent);
	void update(unsigned int ent, T new_data);
	T* get(unsigned int ent);
};

//Insert operation
template<typename T>
void SparseSet<T>::addEntity(unsigned int ent, T d)
{

	if (sparse.size() == 0 || ent > sparse.size() - 1)
	{
		sparse.resize(ent + 1, -1);
	}
	sparse[ent] = (int)dense.size();
	dense.push_back(ent);
	data.push_back(d);
}
//Find operation
template<typename T>
bool SparseSet<T>::find(unsigned int ent)
{
	if (ent < sparse.size() && sparse[ent] != -1)
	{
		return true;
	}
	return false;
}
//Update Operation
template<typename T>
void SparseSet<T>::update(unsigned int ent, T new_data)
{
	if (ent < sparse.size() && sparse[ent] != -1)
	{
		int id = sparse[ent];
		data[id] = new_data;
	}
	else
	{
		this->addEntity(ent, new_data);
	}

}
//Deletion with error if element does not exist
template<typename T>
void SparseSet<T>::remove(unsigned int ent)
{
	if (ent < sparse.size() && sparse[ent] != -1)
	{
		//The id of the dense array which we wish to delete
		int id = sparse[ent];
		int last_ent = dense.back();
		dense[id] = last_ent;
		data[id] = data.back();
		sparse[last_ent] = id;

		dense.pop_back();
		data.pop_back();
		sparse[ent] = -1;

	}
	else
	{
		std::cerr << "[GCERROR] Undefined entry for sparse set" << std::endl;
		abort();
	}
}
//Deletion without error
template<typename T>
void SparseSet<T>::erase(unsigned int ent)
{
	if (ent < sparse.size() && sparse[ent] != -1)
	{
		//The id of the dense array which we wish to delete
		int id = sparse[ent];
		int last_ent = dense.back();
		dense[id] = last_ent;
		data[id] = data.back();
		sparse[last_ent] = id;

		dense.pop_back();
		data.pop_back();
		sparse[ent] = -1;

	}
}

//Getting a value
template<typename T>
T* SparseSet<T>::get(unsigned int ent)
{
	if (ent < sparse.size() && sparse[ent] != -1)
	{
		int id = sparse[ent];
		return &data[id];
	}
	else
	{
		std::cerr << "[GCERROR] Undefined entry for sparse set" << std::endl;
		abort();
	}
}