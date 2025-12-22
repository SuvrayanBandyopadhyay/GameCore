#include"World.hpp"
///<summary>
///Sets the value of a field
///</summary>
unsigned int World::createEntity()
{
	if (free_ids.size() > 0)
	{
		int ret = free_ids.top();
		free_ids.pop();
		return ret;
	}
	return next_id++;
}

///<summary>
///Sets the value of a field
///</summary>
void World::erase(unsigned int ent)
{
	for (auto& i : components)
	{
		i.second->clear(ent);
	}
	free_ids.push(ent);
}