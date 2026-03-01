#include <Harmony/Core/Engine.h>

#include <Harmony/Core/Registry.h>
#include <Harmony/Interfaces/IWindow.h>

#include "Harmony/Core/Service.h"
#include "Harmony/Interfaces/IKernel.h"

namespace Harmony
{
    Engine::Engine(std::unique_ptr<IKernel> kernel) :
        Harmony::Service(*this),
        kernel(std::move(kernel))
    {
    }

    Engine::~Engine() = default;

    void Engine::onInitialize(const Properties& properties)
    {
    }

    void Engine::onUpdate()
    {;
    }

    void Engine::onRender()
    {
    }

    void Engine::onEvent()
    {
    }

    // --- Gestion du cycle de vie du Service ---

    void Engine::onStart()
    {
        logger->info("Engine Service is now running.");
    }

    void Engine::onShutdown()
    {
        logger->info("Engine Service shutting down...");
        onFinalize();
    }

    void Engine::onFinalize()
    {
    }

    void Engine::onPause()  { logger->info("Engine Paused."); }
    void Engine::onResume() { logger->info("Engine Resumed."); }

} // namespace Harmony