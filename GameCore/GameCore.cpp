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
	script.loadString(R"(
function test(a, b)
    return a+b,"Hello",1
end
)");

	script.run("test", 3, 5, 7);

	bool flag = script.getReturn<bool>();        // true
	std::string msg = script.getReturn<std::string>(); // "hello"
	int sum = script.getReturn<int>();          // 12
	
	

	std::cout << sum << "," << msg << "," << flag << std::endl;

}
