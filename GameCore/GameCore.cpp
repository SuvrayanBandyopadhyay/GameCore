/*
This program contains some simple tests which show the overall utility of our ECS system
*/
/*
#include <iostream>
#include<chrono>
#include"World.hpp"
#include"LuaScript.hpp"
#define ELEMENTS 100000
#define FRAMES 10
#define TRIS 10000

//For the lua test
struct Vec2
{
	float x;
	float y;
};
struct tri 
{
	float x1;
	float x2;
	float x3;
};

struct Enemy
{
	Vec2 pos;
	float health;
};

//For the ECS vs OOPs test
struct health 
{
	float h;
};
struct armour 
{
	float a;
};
struct x 
{
	float x;
};
struct y 
{
	float y;
};
struct dx
{
	float dx;
};
struct dy
{
	float dy;
};

struct A {};
struct B {};

//Traditional oops
struct A_OOPS 
{
	float x;
	float y;
	float dx;
	float dy;
	float dmg;
	std::vector<tri>tris;
	float armour;
	float health;

};

struct B_OOPS 
{
	float x;
	float y;
	float dx;
	float dy;
	float dmg;
	std::vector<tri>tris;
	float armour;
	float health;
};

//Definition for Vec2
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
	

	
	LuaScript lua;
	lua.loadFile("enemy.lua");
	Enemy enemy{ {100.0f,50.0f},100.0f };
	lua.run("update_enemy",1, enemy);
	Enemy updated = lua.getReturn<Enemy>();

	printf("Pos: %.1f %.1f\n", updated.pos.x, updated.pos.y);
	printf("Health: %.1f\n", updated.health);
	system("PAUSE");


	std::cout << "Traditional OOPS test \n";

	std::vector<A_OOPS> as(ELEMENTS);
	std::vector<B_OOPS> bs(ELEMENTS);

	for (int i = 0; i < ELEMENTS; i++)
	{
		as[i] = {
			0.0f, 0.0f,
			-0.01f, -0.01f,
			0.01f,
			std::vector<tri>(100, {0.0f, 1.0f, 0.0f}),
			10.0f,
			1000.0f
		};

		bs[i] = {
			0.0f, 0.0f,
			+0.01f, +0.01f,
			0.02f,
			std::vector<tri>(100, {0.0f, 1.0f, 0.0f}),
			10.0f,
			1000.0f
		};
	}

	auto oop_start = std::chrono::steady_clock::now();

	for (int frame = 0; frame < FRAMES; frame++)
	{
		// Movement system
		for (auto& a : as)
		{
			a.x += a.dx;
			a.y += a.dy;
		}

		for (auto& b : bs)
		{
			b.x += b.dx;
			b.y += b.dy;
		}

		// Damage system
		for (auto& a : as)
		{
			if (a.armour > 0)
				a.armour -= a.dmg;
			else
				a.health -= a.dmg;
		}

		for (auto& b : bs)
		{
			if (b.armour > 0)
				b.armour -= b.dmg;
			else
				b.health -= b.dmg;
		}
	}

	auto oop_end = std::chrono::steady_clock::now();
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(oop_end - oop_start);
	std::cout << "Elapsed time: " << milliseconds.count() << " ms\n";
	
	
	system("PAUSE");

	std::cout << "ECS test \n";
	World w;

	for (int i = 0; i < ELEMENTS; i++)
	{
		int a = w.createEntity();
		w.update<x>(a, { 0.0f });
		w.update<y>(a, { 0.0f });
		w.update<dx>(a, { -0.01f });
		w.update<dy>(a, { -0.01f });
		w.update<health>(a, { 1000.0f });
		w.update<armour>(a, { 10.0f });
		w.update<std::vector<tri>>(a, std::vector<tri>(100, { 0.0f,1.0f,0.0f }));
		w.update<A>(a);

		int b = w.createEntity();
		w.update<x>(b, { 0.0f });
		w.update<y>(b, { 0.0f });
		w.update<dx>(b, { +0.01f });
		w.update<dy>(b, { +0.01f });
		w.update<health>(b, { 1000.0f });
		w.update<armour>(b, { 10.0f });
		w.update<std::vector<tri>>(b, std::vector<tri>(100, { 0.0f,1.0f,0.0f }));
		w.update<B>(b);
	}

	View<A> aview(w);
	View<B> bview(w);

	auto ecs_start = std::chrono::steady_clock::now();

	for (int frame = 0; frame < FRAMES; frame++)
	{
		// Movement system
		aview.reset();
		int ent = aview.getNext();
		while (ent != -1)
		{
			auto& px = w.get<x>(ent);
			auto& py = w.get<y>(ent);
			auto& vx = w.get<dx>(ent);
			auto& vy = w.get<dy>(ent);

			px.x += vx.dx;
			py.y += vy.dy;

			ent = aview.getNext();
		}

		bview.reset();
		ent = bview.getNext();
		while (ent != -1)
		{
			auto& px = w.get<x>(ent);
			auto& py = w.get<y>(ent);
			auto& vx = w.get<dx>(ent);
			auto& vy = w.get<dy>(ent);

			px.x += vx.dx;
			py.y += vy.dy;

			ent = bview.getNext();
		}

		// Damage system
		aview.reset();
		ent = aview.getNext();
		while (ent != -1)
		{
			auto& arm = w.get<armour>(ent);
			auto& hp = w.get<health>(ent);

			if (arm.a > 0)
				arm.a -= 0.01f;
			else
				hp.h -= 0.01f;

			ent = aview.getNext();
		}

		bview.reset();
		ent = bview.getNext();
		while (ent != -1)
		{
			auto& arm = w.get<armour>(ent);
			auto& hp = w.get<health>(ent);

			if (arm.a > 0)
				arm.a -= 0.02f;
			else
				hp.h -= 0.02f;

			ent = bview.getNext();
		}
	}

	auto ecs_end = std::chrono::steady_clock::now();
	milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(ecs_end - ecs_start);
	std::cout << "Elapsed time: " << milliseconds.count() << " ms\n";
	system("PAUSE");

}
*/


#include <iostream>
#include<chrono>
#include"World.hpp"
#include"LuaScript.hpp"
#include"View.hpp"
#define ELEMENTS 100000
#define FRAMES 10
#define TRIS 10000
#include <vector>

constexpr int TOTAL_ENTITIES = 500'000;
constexpr int ACTIVE_ENTITIES = 50'000;
constexpr int TRI_COUNT = 100;

struct Timer {
	std::chrono::high_resolution_clock::time_point t;
	Timer() { t = std::chrono::high_resolution_clock::now(); }
	double ms() const {
		return std::chrono::duration<double, std::milli>(
			std::chrono::high_resolution_clock::now() - t
		).count();
	}
};

struct Triangle { float a, b, c; };
struct Position { float x, y; };
struct Velocity { float dx, dy; };
struct Health { float hp; };
struct EnemyTag { char _; };
struct StaticTag { char _; };
struct Mesh { std::vector<Triangle> tris; };

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

//Definition for Vec2
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

struct OOP_Entity {
	Position pos;
	Velocity vel;
	Health   hp;

	bool isEnemy;
	bool isStatic;

	// BIG cache poison
	std::vector<Triangle> mesh;
};

void oop_test()
{
	std::vector<OOP_Entity> entities(TOTAL_ENTITIES);

	for (int i = 0; i < TOTAL_ENTITIES; i++) {
		entities[i].pos = { 0, 0 };
		entities[i].vel = { 1, 1 };
		entities[i].hp = { 100 };

		entities[i].isEnemy = i < ACTIVE_ENTITIES;
		entities[i].isStatic = !entities[i].isEnemy;


		entities[i].mesh.resize(TRI_COUNT);
	}

	Timer t;

	for (int f = 0; f < FRAMES; f++) {

		for (auto& e : entities) {
			if (!e.isStatic) {
				e.pos.x += e.vel.dx;
				e.pos.y += e.vel.dy;
			}
		}


		for (auto& e : entities) {
			if (e.isEnemy && e.hp.hp < 50) {
				e.pos.x += 0.5f;
			}
		}
	}

	std::cout << "Traditional OOP test\n";
	std::cout << "Elapsed time: " << t.ms() << " ms\n\n";
}

void ecs_test()
{
	World w;

	for (int i = 0; i < TOTAL_ENTITIES - ACTIVE_ENTITIES; i++) {
		auto e = w.createEntity();
		w.update<Position>(e, { 0, 0 });
		w.update<Mesh>(e, { std::vector<Triangle>(TRI_COUNT) });
		w.update<StaticTag>(e);
	}

	for (int i = 0; i < ACTIVE_ENTITIES; i++) {
		auto e = w.createEntity();
		w.update<Position>(e, { 0, 0 });
		w.update<Velocity>(e, { 1, 1 });
		w.update<Health>(e, { 100 });
		w.update<EnemyTag>(e);
		w.update<Mesh>(e, { std::vector<Triangle>(TRI_COUNT) });
	}

	Timer t;

	for (int f = 0; f < FRAMES; f++) {

		for (auto [pos, vel] : View<Position, Velocity>(w)) {
			pos.x += vel.dx;
			pos.y += vel.dy;
		}


		for (auto [pos, hp] : View<Position, Health>(w)) {
			if (hp.hp < 50) {
				pos.x += 0.5f;
			}
		}
	}

	std::cout << "ECS test\n";
	std::cout << "Elapsed time: " << t.ms() << " ms\n\n";
}

int main()
{
	//oops vs ecs
	oop_test();
	ecs_test();
	//Lua test
	system("PAUSE");

	//Standard tests
	std::cout << "LUA TESTS\n";
	LuaScript script;
	script.loadFile("l.lua");
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
	lua.run("update_enemy", 1, enemy);
	Enemy updated = lua.getReturn<Enemy>();

	printf("Pos: %.1f %.1f\n", updated.pos.x, updated.pos.y);
	printf("Health after update: %.1f\n", updated.health);
	system("PAUSE");
}