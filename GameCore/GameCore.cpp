// GameCore.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<chrono>
#include"World.hpp"
#include"LuaScript.hpp"

struct health 
{
	float h;
};

struct damage 
{
	float d;
};

struct Vec2 
{
	float x;
	float y;
};

struct Enemy 
{
	Vec2 pos;
	float health;
};

template<>
struct LuaType<Vec2> 
{
	static void push(lua_State* L, const Vec2& v)
	{
		lua_newtable(L);

		LuaValue<float>::push(L, v.x, "x");
		LuaValue<float>::push(L, v.y, "y");
	}

	static Vec2 get(lua_State* L, int index)
	{
		luaL_checktype(L, index, LUA_TTABLE);
		Vec2 v{};

		lua_getfield(L, index, "x");
		v.x = LuaValue<float>::get(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, index, "y");
		v.y = LuaValue<float>::get(L, -1);
		lua_pop(L, 1);

		return v;
	}
};

template<>
struct LuaType<Enemy>
{
	static void push(lua_State* L, const Enemy& e)
	{
		lua_newtable(L);

		LuaValue<Vec2>::push(L, e.pos, "pos");
		LuaValue<float>::push(L, e.health, "health");
	}

	static Enemy get(lua_State* L, int index)
	{
		luaL_checktype(L, index, LUA_TTABLE);
		Enemy e{};

		lua_getfield(L, index, "pos");
		e.pos = LuaValue<Vec2>::get(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, index, "health");
		e.health = LuaValue<float>::get(L, -1);
		lua_pop(L, 1);

		return e;
	}
};

class pos 
{
	
public:
	float x;
	float y;
	pos() 
	{
		x = 0;
		y = 0;
	}
	pos(float a,float b) 
	{
		x = a;
		y = b;
	}
};

int main()
{
	World w;
	int ent = -1;
	auto start = std::chrono::steady_clock::now();
	const float limit = 10000000;
	for (int i = 0; i < limit; i++) 
	{
		ent = w.createEntity();
		w.update<health>(ent, health{ 100.32+32.2+11.2 });
	}
	auto end = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	std::cout << "Elapsed time: " << elapsed_ms.count() << " nanoseconds" << std::endl;
	std::cout << "Average time: " << elapsed_ms.count()/limit << " nanoseconds" << std::endl;

	ent = -1;
	start = std::chrono::steady_clock::now();
	auto& h = w.getComponent<health>();
	for (int i = 0; i < h.dense.size(); i++)
	{
		int ent = h.dense[i];
		h.update(ent, health{ 131 });
	}
	end = std::chrono::steady_clock::now();
	elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	std::cout << "Elapsed time: " << elapsed_ms.count() << " nanoseconds" << std::endl;
	std::cout << "Average time: " << elapsed_ms.count() / limit << " nanoseconds" << std::endl;

	LuaScript script;
	script.loadFile("C:/Users/Dell/Documents/Programming/Game Development/GameCore/x64/Debug/l.lua");
	script.run("test", 3, 51, 7);

	bool flag = script.getReturn<bool>();        // true
	std::string msg = script.getReturn<std::string>(); // "hello"
	int sum = script.getReturn<int>();          // 12
	std::cout << sum << "," << msg << "," << flag << std::endl;

	script.run("test", 3, 52, 7);
	flag = script.getReturn<bool>();        // true
	msg = script.getReturn<std::string>(); // "hello"
	sum = script.getReturn<int>();          // 12


	std::cout << sum << "," << msg << "," << flag << std::endl;

	
	LuaScript lua;
	lua.loadFile("enemy.lua");
	Enemy enemy{ {100.0f,50.0f},100.0f };
	lua.run("update_enemy",1, enemy);
	Enemy updated = lua.getReturn<Enemy>();

	printf("Pos: %.1f %.1f\n", updated.pos.x, updated.pos.y);
	printf("Health: %.1f\n", updated.health);
	
	pos p(2,3);
	w.setField<pos>(p);
	std::cout<<w.getField<pos>().x << std::endl;
	w.deleteField<pos>();
	if(!w.fieldExists<pos>())
	{
		std::cout << "SUCCESS" << std::endl;
		w.setField<pos>(p);
		std::cout << w.getField<pos>().x << std::endl;
	}
	
	system("PAUSE");
}
