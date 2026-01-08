#include"LuaScript.hpp"

LuaScript::LuaScript()
{
	L = luaL_newstate();
	luaL_openlibs(L);
	type = 0;
}

LuaScript::~LuaScript() 
{
	lua_close(L);
}

bool LuaScript::loadString(std::string code) 
{
	if (luaL_loadstring(L, code.c_str()) != LUA_OK) 
	{
		std::cerr << "Lua error " << lua_tostring(L, -1) << "\n";
		lua_pop(L, 1);
		return false;
	}
	lua_pcall(L, 0, LUA_MULTRET, 0);
	type = 0;
	return true;
}

bool LuaScript::loadFile(std::string filename)
{
	if (luaL_loadfile(L, filename.c_str()) != LUA_OK)
	{
		std::cerr << "Lua error " << lua_tostring(L, -1) << "\n";
		lua_pop(L, 1);
		return false;
	}
	lua_pcall(L, 0, LUA_MULTRET, 0);
	type = 1;
	return true;
}

lua_State* LuaScript::state()
{
	return L;
}