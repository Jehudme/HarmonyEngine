#include <Harmony/Core/Service.h>
#include "Harmony/Core/ContextLogger.h"

namespace Harmony {

    Service::Service(const std::string& name, const std::string& type, Engine& engine)
        : Extension(name, type, engine) {
    }

    // ========================================================================
    // Lifecycle Management
    // ========================================================================

    void Service::start() {
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;

        // Validate transition: Only an 'Initialized' service can move to 'Running'.
        const bool canStart = m_state.read([this](const State& currentState) -> bool {
            if (currentState == State::Running) {
                m_logger->warn("Service '{}' of type '{}' is already running; ignoring start() call.", m_name, m_type);
                return false;
            }
            if (currentState == State::Shutdown) {
                m_logger->error("Cannot start service '{}': it has been shut down permanently.", m_name);
                return false;
            }
            if (currentState == State::Paused) {
                m_logger->error("Service '{}' is paused; use resume() to continue execution.", m_name);
                return false;
            }
            return currentState == State::Initialized;
        });

        if (!canStart) return;

        {
            // Atomically update state and trigger the user-defined startup hook.
            // Lock ensures no concurrent state transitions during initialization.
            std::lock_guard<std::mutex> lock(m_serviceMutex);
            m_state.write([this](State& stateRef) {
                stateRef = State::Running;
                onStart();
            });
            m_isThreadManaged = true;
            m_logger->info("Service '{}' of type '{}' transitioning to Running state.", m_name, m_type);
        }

        // Thread creation occurs outside the lock to prevent the new thread
        // from immediately contending for the mutex we just released.
        m_thread = std::thread(&Service::run, this);
    }

    void Service::stop() {
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;

        const bool canStop = m_state.read([this](const State& currentState) -> bool {
            if (currentState == State::Shutdown) {
                m_logger->warn("Service '{}' of type '{}' is already shut down.", m_name, m_type);
                return false;
            }
            return true;
        });

        if (!canStop) return;

        {
            std::lock_guard<std::mutex> lock(m_serviceMutex);
            m_state.write([this](State& stateRef) {
                stateRef = State::Shutdown;
                onShutdown();
            });
            m_isThreadManaged = false;
            m_logger->info("Service '{}' of type '{}' transitioning to Shutdown state.", m_name, m_type);
        }

        // Wake the thread if it's currently idling in a 'Paused' state, so it
        // can observe the shutdown signal and exit cleanly.
        m_conditionVariable.notify_all();

        // Join outside of locks to prevent deadlocks where the worker thread
        // tries to acquire a lock the main thread is still holding.
        if (m_thread.joinable()) {
            m_logger->trace("Service '{}': Waiting for worker thread to join...", m_name);
            m_thread.join();
            m_logger->trace("Service '{}': Worker thread has joined.", m_name);
        }
    }

    void Service::pause() {
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;

        if (!m_isThreadManaged) {
            m_logger->warn("Service '{}' has no managed thread; pause() has no effect.", m_name);
            return;
        }

        const bool canPause = m_state.read([this](const State& currentState) -> bool {
            if (currentState != State::Running) {
                m_logger->error("Service '{}' must be Running to be paused; current state does not allow pause.", m_name);
                return false;
            }
            return true;
        });

        if (canPause) {
            std::lock_guard<std::mutex> lock(m_serviceMutex);
            m_state.write([this](State& stateRef) {
                stateRef = State::Paused;
                onPause();
            });
            m_logger->info("Service '{}' of type '{}' transitioning to Paused state.", m_name, m_type);
        }
    }

    void Service::resume() {
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;

        if (!m_isThreadManaged) {
            m_logger->warn("Service '{}' has no managed thread; resume() has no effect.", m_name);
            return;
        }

        const bool canResume = m_state.read([this](const State& currentState) -> bool {
            if (currentState != State::Paused) {
                m_logger->error("Service '{}' must be Paused to be resumed; current state does not allow resume.", m_name);
                return false;
            }
            return true;
        });

        if (canResume) {
            {
                std::lock_guard<std::mutex> lock(m_serviceMutex);
                m_state.write([this](State& stateRef) {
                    stateRef = State::Running;
                    onResume();
                });
                m_logger->info("Service '{}' of type '{}' transitioning to Running state from Paused.", m_name, m_type);
            }
            // Wake the run loop so it can resume execution.
            m_conditionVariable.notify_all();
        }
    }

    // ========================================================================
    // Internal Execution
    // ========================================================================

    void Service::run() {
        // Push the service's logger onto the thread-local context stack for the
        // duration of the run loop. Any nested Logger::context() calls resolve here.
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;

        m_state.write([](State& stateRef) { stateRef = State::Running; });
        m_logger->trace("Service '{}' run loop started on dedicated thread.", m_name);

        while (true) {
            const State currentState = m_state.read([](const State& stateRef) { return stateRef; });

            if (currentState == State::Shutdown) {
                m_logger->trace("Service '{}' detected Shutdown state; exiting run loop.", m_name);
                break;
            }

            if (currentState == State::Paused) {
                // Block until the state is no longer Paused. The condition variable
                // prevents busy-waiting while paused.
                std::unique_lock<std::mutex> pauseLock(m_serviceMutex);
                m_conditionVariable.wait(pauseLock, [this] {
                    return m_state.read([](const State& stateRef) { return stateRef != State::Paused; });
                });
                continue; // Re-evaluate state immediately after waking.
            }

            // Standard Execution Pulse: update, render, and process events.
            if (currentState == State::Running) {
                onUpdate();
                onRender();
                onEvent();
            }
        }

        m_logger->trace("Service '{}' run loop has exited.", m_name);
    }

} // namespace Harmony