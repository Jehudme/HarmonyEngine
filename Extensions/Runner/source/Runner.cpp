#include "Runner.h"

#include "Harmony/Core/Registry.h"
#include "Harmony/Interfaces/IKernel.h"
#include "Harmony/Interfaces/IWindow.h"
#include "Harmony/Core/ContextLogger.h"

#include <chrono>
#include <stdexcept>

namespace Harmony
{
    Runner::~Runner()
    {
        stop();
    }

    void Runner::start()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        State expected = State::Initialized;

        if (m_state.compare_exchange_strong(expected, State::Running))
        {
            m_logger->info("Runner starting: Launching kernel loop in background thread.");
            m_workerThread = std::thread(&Runner::run, this);
        }
        else
        {
            // Error handling for improper state calls
            if (expected == State::Running) {
                m_logger->warn("Runner is already running. Start request ignored.");
            } else if (expected == State::Paused) {
                m_logger->warn("Runner is currently paused. Use resume() instead of start().");
            } else if (expected == State::Shutdown) {
                m_logger->error("Runner has already been shut down. Cannot restart.");
            }
        }
    }

    void Runner::run()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->info("Runner background thread started successfully.");

        try {
            // Loop safely checks the atomic state
            while (m_state.load(std::memory_order_acquire) != State::Shutdown)
            {
                // Fail-safe: Check if the user clicked the 'X' on the window
                if (m_kernel.extension<IWindow>()->shouldClose()) {
                    m_logger->info("Window close requested. Initiating Runner shutdown.");
                    m_state.store(State::Shutdown, std::memory_order_release);
                    break;
                }

                State currentState = m_state.load(std::memory_order_acquire);

                if (currentState == State::Running)
                {
                    m_kernel.progress();
                }
                else if (currentState == State::Paused)
                {
                    // Fail-safe: Prevent the thread from eating 100% CPU when the game is paused
                    std::this_thread::sleep_for(std::chrono::milliseconds(16));
                }
            }
        }
        catch (const std::exception& e) {
            m_logger->critical("Runner crashed with exception: {}", e.what());
        }
        catch (...) {
            m_logger->critical("Runner crashed with an unknown fatal error.");
        }

        m_logger->info("Runner background thread exiting.");
    }

    void Runner::stop()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());

        // exchange() forcefully sets Shutdown and returns the previous state
        State oldState = m_state.exchange(State::Shutdown, std::memory_order_acq_rel);
        if (oldState == State::Shutdown) {
            return; // Already shutting down.
        }

        m_logger->info("Runner stop requested. Waiting for background thread to finish...");

        if (m_workerThread.joinable())
        {
            // CRITICAL FAIL-SAFE: Deadlock prevention.
            // If the window is closed from INSIDE the run() thread, and it triggers stop(),
            // calling .join() on itself will cause a std::system_error crash.
            if (std::this_thread::get_id() != m_workerThread.get_id()) {
                m_workerThread.join();
                m_logger->info("Runner background thread joined successfully.");
            } else {
                // If we are shutting down from inside the thread itself, detach it to let it die naturally.
                m_workerThread.detach();
                m_logger->warn("Runner stopped from within its own thread. Thread detached.");
            }
        }
    }

    void Runner::pause()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());

        State expected = State::Running;
        if (m_state.compare_exchange_strong(expected, State::Paused)) {
            m_logger->info("Runner paused.");
        } else {
            if (expected == State::Paused) m_logger->warn("Runner is already paused.");
            else m_logger->error("Cannot pause: Runner is not currently running.");
        }
    }

    void Runner::resume()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());

        State expected = State::Paused;
        if (m_state.compare_exchange_strong(expected, State::Running)) {
            m_logger->info("Runner resumed.");
        } else {
            if (expected == State::Running) m_logger->warn("Runner is already running.");
            else m_logger->error("Cannot resume: Runner is not currently paused.");
        }
    }

    // ==========================================
    // Lifecycle Implementations
    // ==========================================
    void Runner::onInitialize(const Properties&) {}
    void Runner::onFinalize() {}
    void Runner::onUpdate() {}
    void Runner::onRender() {}
    void Runner::onEvent() {}

    HARMONY_REGISTER_EXTENSION(Runner)
} // Harmony