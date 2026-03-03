#include "Runner.h"

#include "Harmony/Core/Registry.h"
#include "Harmony/Interfaces/IKernel.h"
#include "Harmony/Interfaces/IWindow.h"

namespace Harmony
{
    void Runner::start()
    {
    }

    void Runner::run()
    {
        while (!m_kernel.extension<IWindow>()->shouldClose())
        {
            m_kernel.progress();
        }
    }

    void Runner::stop()
    {
    }

    void Runner::pause()
    {
    }

    void Runner::resume()
    {
    }

    HARMONY_REGISTER_EXTENSION(Runner)
} // Harmony