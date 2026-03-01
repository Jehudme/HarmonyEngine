#include <Harmony/Core/Engine.h>

#include <Harmony/Core/Registry.h>
#include <Harmony/Interfaces/IWindow.h>

#include "Harmony/Core/Service.h"
#include "Harmony/Interfaces/IKernel.h"
#include "Harmony/Core/ContextLogger.h"

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
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;
        m_logger->info("Service '{}' of type '{}' has started. Game loop is now running.", m_name, m_type);
    }

    void Engine::onShutdown()
    {
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;
        m_logger->info("Service '{}' of type '{}' received shutdown signal. Terminating game loop...", m_name, m_type);
    }

    void Engine::onFinalize()
    {
        kernel->finalize();
    }

    void Engine::onPause()  { HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD; m_logger->info("Service '{}' of type '{}' has been paused.", m_name, m_type); }
    void Engine::onResume() { HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD; m_logger->info("Service '{}' of type '{}' has been resumed.", m_name, m_type); }

} // namespace Harmony