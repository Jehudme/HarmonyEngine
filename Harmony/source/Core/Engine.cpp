#include <Harmony/Core/Engine.h>

#include <Harmony/Core/Registry.h>
#include <Harmony/Interfaces/IWindow.h>

#include "Harmony/Core/Service.h"
#include "Harmony/Interfaces/IKernel.h"

namespace Harmony
{
    Engine::Engine(std::unique_ptr<IKernel> kernel) :
        Harmony::Service("engine", "engine", *this),
        kernel(std::move(kernel))
    {
    }

    Engine::~Engine() {
        Controller::finalize();
    }

    void Engine::onInitialize(const Properties& properties)
    {
        kernel->initialize(*this, properties);
    }

    void Engine::onUpdate()
    {
        kernel->update();
    }

    void Engine::onRender()
    {
        kernel->render();
    }

    void Engine::onEvent()
    {
        kernel->event();
    }

    // --- Gestion du cycle de vie du Service ---

    void Engine::onStart()
    {
        logger->info("Engine Service is now running.");
    }

    void Engine::onShutdown()
    {
        logger->info("Engine Service shutting down...");
    }

    void Engine::onFinalize()
    {
        kernel->finalize();
    }

    void Engine::onPause()  { logger->info("Engine Paused."); }
    void Engine::onResume() { logger->info("Engine Resumed."); }

} // namespace Harmony