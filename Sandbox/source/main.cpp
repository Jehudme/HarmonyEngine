#include <iostream>
#include <thread>
#include <chrono>
#include <Harmony/Harmony.h>

int main()
{
    Harmony::Properties properties;
    properties.load("assets/application.json");

    auto kernel = std::make_unique<Harmony::Kernel>();
    Harmony::Engine engine(std::move(kernel));

    // Initializes the engine and loads all modules dynamically via the registry
    engine.initialize(properties);

    // Engine inherits from Service. start() launches the game loop thread.
    engine.run();

    // Gracefully shut down the engine loop and join the thread
    engine.stop();
    return 0;
}