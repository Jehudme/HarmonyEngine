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
        Extension::finalize();
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

    void Engine::onStart()
    {
        m_logger->info("Engine service is now running.");
    }

    void Engine::onShutdown()
    {
        m_logger->info("Engine service is shutting down...");
    }

    void Engine::onFinalize()
    {
        kernel->finalize();
    }

    void Engine::onPause()  { m_logger->info("Engine service paused."); }
    void Engine::onResume() { m_logger->info("Engine service resumed."); }

} // namespace Harmony