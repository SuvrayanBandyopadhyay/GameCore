#pragma once
#include<iostream>
#include<string>
extern "C"
{
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

class LuaScript
{
private:
	lua_State* L;
	char* script;//Can be a string or a filename
	bool type; //0-> String 1-> File
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
};

template<typename T, typename... Args>
void LuaScript::pushToLua(T value, Args... args)
{
	//If its arithmetic
	if constexpr (std::is_arithmetic_v<T>)
	{
		lua_pushnumber(L, static_cast<lua_Number>(value));
	}
	//For bools
	else if constexpr (std::is_same_v<T, bool>)
	{
		lua_pushboolean(L, value);
	}
	//For strings
	else if constexpr (std::is_same_v<T, std::string>)
	{
		lua_pushstring(L, value.c_str());
	}
	//For const char *
	else if constexpr (std::is_same_v<T, const char *>)
	{
		lua_pushstring(L, value);
	}
	else
	{
		//Use static assert to print an error
		static_assert(sizeof(T) == 0, "Unsupported type for Lua push");
	}
	//For the remaining args
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
	T ret{};
	if constexpr (std::is_arithmetic_v<T>)
	{
		if (lua_isnumber(L, -1))
		{
			ret = static_cast<T>(lua_tonumber(L, -1));
		}
	}
	//For bools
	else if constexpr (std::is_same_v<T, bool>)
	{
		if (lua_isboolean(L, -1))
		{
			ret = static_cast<T>(lua_toboolean(L, -1));
		};
	}
	//For strings
	else if constexpr (std::is_same_v<T, std::string>)
	{
		if (lua_isstring(L, -1))
		{
			ret = static_cast<T>(lua_tostring(L, -1));
		}
	}
	else
	{
		//Use static assert to print an error
		static_assert(sizeof(T) == 0, "Unsupported type for Lua push");
	}
	lua_pop(L, 1);
	return ret;
}