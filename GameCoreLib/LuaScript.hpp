#pragma once
#include<iostream>
#include<string>
extern "C"
{
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}
//Declaration for generic lua objects
template<typename T>
struct LuaType 
{
	static void push(lua_State* L, const T&) 
	{
		static_assert(sizeof(T) == 0, "LuaType<T>::push not implemneted for this type");
	}
	static void get(lua_State* L, const T&)
	{
		static_assert(sizeof(T) == 0, "LuaType<T>::get not implemneted for this type");
	}
};

//Declaration for LuaValues
template<typename T>
struct LuaValue 
{
	static void push(lua_State* L, const T& v) 
	{
		if constexpr (std::is_arithmetic_v<T>)
			lua_pushnumber(L, (lua_Number)v);
		else if constexpr (std::is_same_v<T, bool>)
			lua_pushboolean(L, (lua_Number)v);
		else if constexpr (std::is_same_v<T, std::string>)
			lua_pushstring(L, v.c_str());
		else
			LuaType<T>::push(L, v);//Push a struct
	}
	//Push with name
	static void push(lua_State* L, const T& v, std::string name) 
	{
		push(L, v);
		lua_setfield(L, -2, name.c_str());
	}
	//Get value
	static T get(lua_State* L, int idx) 
	{
		if constexpr (std::is_arithmetic_v<T>) 
		{
			return (T)luaL_checknumber(L, idx);
		}
		else if constexpr (std::is_same_v<T, bool>) 
		{
			return lua_toboolean(L, idx);
		}
		else if constexpr (std::is_same_v<T, std::string>) 
		{
			return luaL_checkstring(L, idx);
		}
		else 
		{
			return LuaType<T>::get(L, idx);
		}
	}
};

//Script class
class LuaScript
{
private:
	lua_State* L;
	char* script;//Can be a string or a filename
	bool type; //0-> String 1-> File
	template<typename T>
	void pushOne(const T& value);
	template<typename T, typename... Args>
	void pushToLua(T value, Args... args);
public:
	LuaScript();
	~LuaScript();
	bool loadString(std::string code);
	bool loadFile(std::string filename);
	template<typename... Args>
	void run(std::string function, int rets, Args... args);
	template<typename Ret>
	Ret getReturn();
	lua_State* state();
};
template<typename T>
void LuaScript::pushOne(const T& value) 
{
	LuaValue<T>::push(L, value);
}

template<typename T, typename... Args>
void LuaScript::pushToLua(T value, Args... args)
{
	pushOne(value);
	if constexpr (sizeof...(args) > 0) 
	{
		pushToLua(args...);
	}
}

template<typename... Args>
void LuaScript::run(std::string funcName,int rets,Args... args) 
{
	
	lua_getglobal(L, funcName.c_str());
	pushToLua(args...);

	const int numArgs = sizeof...(Args);
	if (lua_pcall(L, numArgs, rets, 0) != LUA_OK) 
	{
		std::cerr << "Lua error " << lua_tostring(L, -1) << "\n";
		lua_pop(L, 1);
		throw std::runtime_error("Lua function call failed");
	}

}

template<typename T>
T LuaScript::getReturn() 
{
	T value = LuaValue<T>::get(L, -1);
	lua_pop(L, 1);
	return value;
}

