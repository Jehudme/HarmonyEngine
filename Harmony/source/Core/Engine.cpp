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
        m_kernel(std::move(kernel))
    {
    }

    Engine::~Engine() {
        Extension::finalize();
    }

    void Engine::onInitialize(const Properties& properties)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->info("Engine '{}' initializing kernel with provided properties...", m_name);
        m_kernel->initialize(*this, properties);
        m_logger->info("Engine '{}' kernel initialization complete.", m_name);
    }

    void Engine::onUpdate(float deltaTime)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_kernel->update(deltaTime);
    }

    void Engine::onRender()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_kernel->render();
    }

    void Engine::onEvent()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_kernel->event();
    }

    void Engine::onStart()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->info("Engine '{}' of type '{}' has started. Main game loop is now active.", m_name, m_type);
    }

    void Engine::onShutdown()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->info("Engine '{}' of type '{}' received shutdown signal. Terminating main game loop...", m_name, m_type);
    }

    void Engine::onFinalize()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->info("Engine '{}' finalizing kernel...", m_name);
        m_kernel->finalize();
        m_logger->info("Engine '{}' kernel finalization complete.", m_name);
    }

    void Engine::onPause()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->info("Engine '{}' of type '{}' has been paused.", m_name, m_type);
    }

    void Engine::onResume()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->info("Engine '{}' of type '{}' has been resumed.", m_name, m_type);
    }

} // namespace Harmony