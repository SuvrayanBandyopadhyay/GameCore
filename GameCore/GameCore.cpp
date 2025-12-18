#include <chrono>
#include <iostream>
#include <iomanip>
#include "World.hpp"

// Test components
struct Health { float value; };
struct Position { float x, y; };
struct Velocity { float dx, dy; };
struct Damage { float value; };

int main() {
    World world;
    const int NUM_ENTITIES = 10000;
    const int ITERATIONS = 100000;

    std::cout << "=== ECS Benchmark ===" << std::endl;
    std::cout << "Setting up " << NUM_ENTITIES << " entities..." << std::endl;

    // Setup entities
    for (int i = 0; i < NUM_ENTITIES; i++) {
        world.update(i, Health{ 100.0f });
        world.update(i, Position{ 0.0f, 0.0f });
        world.update(i, Velocity{ 1.0f, 1.0f });
        world.update(i, Damage{ 25.0f });
    }

    std::cout << "\nRunning benchmarks...\n" << std::endl;

    // Benchmark 1: getComponent
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; i++) {
            volatile auto& comp = world.getComponent<Health>();
        }
        auto end = std::chrono::high_resolution_clock::now();
        double ns = std::chrono::duration<double, std::nano>(end - start).count();
        std::cout << std::setw(30) << std::left << "getComponent<Health>"
            << std::setw(10) << std::right << (ns / ITERATIONS) << " ns/op" << std::endl;
    }

    // Benchmark 2: Get specific entity component
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; i++) {
            volatile float val = world.get<Health>(5000).value;
        }
        auto end = std::chrono::high_resolution_clock::now();
        double ns = std::chrono::duration<double, std::nano>(end - start).count();
        std::cout << std::setw(30) << std::left << "get<Health>(entity)"
            << std::setw(10) << std::right << (ns / ITERATIONS) << " ns/op" << std::endl;
    }

    // Benchmark 3: Update component
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; i++) {
            world.update(5000, Health{ 95.0f });
        }
        auto end = std::chrono::high_resolution_clock::now();
        double ns = std::chrono::duration<double, std::nano>(end - start).count();
        std::cout << std::setw(30) << std::left << "update<Health>"
            << std::setw(10) << std::right << (ns / ITERATIONS) << " ns/op" << std::endl;
    }

    // Benchmark 4: Multiple component access
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; i++) {
            volatile auto& h = world.getComponent<Health>();
            volatile auto& p = world.getComponent<Position>();
            volatile auto& v = world.getComponent<Velocity>();
            volatile auto& d = world.getComponent<Damage>();
        }
        auto end = std::chrono::high_resolution_clock::now();
        double ns = std::chrono::duration<double, std::nano>(end - start).count();
        std::cout << std::setw(30) << std::left << "Multi-component access"
            << std::setw(10) << std::right << (ns / ITERATIONS) << " ns/op" << std::endl;
    }

    // Benchmark 5: Field get
    world.setField<float>(9.8f);
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; i++) {
            volatile float g = world.getField<float>();
        }
        auto end = std::chrono::high_resolution_clock::now();
        double ns = std::chrono::duration<double, std::nano>(end - start).count();
        std::cout << std::setw(30) << std::left << "getField<float>"
            << std::setw(10) << std::right << (ns / ITERATIONS) << " ns/op" << std::endl;
    }

    // Benchmark 6: Field set
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; i++) {
            world.setField<float>(9.8f);
        }
        auto end = std::chrono::high_resolution_clock::now();
        double ns = std::chrono::duration<double, std::nano>(end - start).count();
        std::cout << std::setw(30) << std::left << "setField<float>"
            << std::setw(10) << std::right << (ns / ITERATIONS) << " ns/op" << std::endl;
    }

    // Benchmark 7: Realistic system - update positions
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int iter = 0; iter < 1000; iter++) {
            auto& positions = world.getComponent<Position>();
            auto& velocities = world.getComponent<Velocity>();

            for (int i = 0; i < NUM_ENTITIES; i++) {
                if (positions.find(i) && velocities.find(i)) {
                    auto pos = positions.get(i);
                    auto vel = velocities.get(i);
                    pos->x += vel->dx;
                    pos->y += vel->dy;
                }
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        double ns = std::chrono::duration<double, std::nano>(end - start).count();
        std::cout << std::setw(30) << std::left << "System: Update positions"
            << std::setw(10) << std::right << (ns / 1000) << " ns/op" << std::endl;
    }

    // Benchmark 8: Apply damage system
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int iter = 0; iter < 1000; iter++) {
            auto& healths = world.getComponent<Health>();
            auto& damages = world.getComponent<Damage>();

            for (int i = 0; i < NUM_ENTITIES; i++) {
                if (healths.find(i) && damages.find(i)) {
                    auto h = healths.get(i);
                    auto d = damages.get(i);
                    h->value -= d->value * 0.01f;
                }
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        double ns = std::chrono::duration<double, std::nano>(end - start).count();
        std::cout << std::setw(30) << std::left << "System: Apply damage"
            << std::setw(10) << std::right << (ns / 1000) << " ns/op" << std::endl;
    }

    std::cout << "\n=== Expected Results ===" << std::endl;
    std::cout << "getComponent:        ~2-5 ns   (static caching)" << std::endl;
    std::cout << "get entity:          ~10-30 ns (sparse set lookup)" << std::endl;
    std::cout << "getField/setField:   ~1-3 ns   (static caching)" << std::endl;
    system("pause");
    return 0;
}