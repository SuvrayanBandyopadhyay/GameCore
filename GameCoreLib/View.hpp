#include<iostream>
#include"World.hpp"

///<summary>
///Views allow us to easily select entities which have and dont have certain components from a world 
/// </summary>
/*
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
*/

template<typename... Comps>
class View
{
private:
	//The world which we are currently working with
	World* w;
	//The component which is driving our view for efficiency
	std::vector<int>* driver_dense = nullptr;
	std::vector<int*>sparse_maps;
	//The arrays of data returned
	std::tuple<std::vector<Comps>*...> data_arrays;

	//Function to select driver
	template<typename T>
	void select_driver()
	{
		auto& comp = w->getComponent<T>();
		//If no driver or component smaller than current drived
		if (!driver_dense || comp.dense.size() < driver_dense->size())
		{
			driver_dense = &comp.dense;
		}
	}

	//Function to cache entries
	template<typename T>
	void cache_component()
	{
		auto& comp = w->getComponent<T>();
		sparse_maps.push_back(comp.sparse.data());
		std::get<std::vector<T>*>(data_arrays) = &comp.data;
	}

	auto get_tuple(int entity) 
	{
		//Return a tuple of values using values cached in data_arrays
		return std::tuple<Comps&...>(
			std::get<std::vector<Comps>*>(data_arrays)->at(
			w->getComponent<Comps>().sparse[entity]
			)...
		);
	}
public:
	View(World& world) : w(&world)
	{
		(select_driver<Comps>(), ...);
		(cache_component<Comps>(), ...);
	}
	//Defining an iterator to iterate through the elements
	class Iterator 
	{
	private:
		View* view;
		size_t idx;
	public:
		//Skip invalid elements
		void skip_invalid()
		{
			while (idx < view->driver_dense->size()) 
			{
				int e = (*view->driver_dense)[idx];
				bool valid = true;
				for (int* sparse : view->sparse_maps) 
				{
					if (sparse[e] == -1) 
					{
						valid = false;
						break;
					}
				}
				if (valid) 
				{
					break;
				}
				idx += 1;
			}
		}
		//Constructor
		Iterator(View* v, size_t i) 
		{
			view = v;
			idx = i;
			skip_invalid();
		}
		//++ operator
		Iterator& operator++() 
		{
			idx++;
			skip_invalid();
			return *this;
		}
		//!= operator
		bool operator!=(const Iterator& o) 
		{
			return idx != o.idx;
		}
		//* operator
		auto operator*() 
		{
			int e = (*view->driver_dense)[idx];
			return view->get_tuple(e);
		}
	};
	Iterator begin() { return Iterator(this, 0); }
	Iterator end() { return Iterator(this, driver_dense->size()); }
};