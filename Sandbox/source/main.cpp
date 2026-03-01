#include <iostream>
#include <Harmony/Harmony.h>

int main()
{
    Harmony::Properties properties;
    properties.load("assets/application.json");

    auto kernel = std::make_unique<Harmony::Kernal>();
    Harmony::Engine engine(std::move(kernel));
    engine.initialize(properties);

    auto& window = static_cast<Harmony::IWindow&>(engine.kernel->controller("window"));

    while (!window.shouldClose())
    {
        engine.update();
        engine.render();
        window.swapBuffers();
    }

    engine.finalize();
    return 0;
}