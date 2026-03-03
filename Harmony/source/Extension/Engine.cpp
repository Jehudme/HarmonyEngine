#include <Extension/Engine.h>
#include <Harmony/Core/Registry.h>
#include <Harmony/Interfaces/IWindow.h>

#include "Harmony/Interfaces/IKernel.h"
#include "Harmony/Core/ContextLogger.h"
#include "Harmony/Interfaces/IKernel.h"
#include "Harmony/Core/Extension.h"


namespace Harmony
{
    Engine::Engine(IKernel& kernel) : IRunner("engine", kernel)
    {
    }

    void Engine::start()
    {
    }

    void Engine::run()
    {
        while(true)
        {
            m_kernel->progress();
        }
    }

    void Engine::stop()
    {
    }

    void Engine::pause()
    {
    }

    void Engine::resume()
    {
    }

    void Engine::onInitialize(const Properties& properties)
    {
    }


    void Engine::onFinalize()
    {
    }

    void Engine::onRender()
    {
    }

    void Engine::onUpdate()
    {
    }

    void Engine::onEvent()
    {
    }

    HARMONY_REGISTER(Extension, Engine, "engine", IKernel&);

} // namespace Harmony