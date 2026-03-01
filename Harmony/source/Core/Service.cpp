#include <Harmony/Core/Service.h>
#include "Harmony/Core/ContextLogger.h"

namespace Harmony {

    Service::Service(const std::string& name, const std::string& type, Engine& engine) : Extension(name, type, engine) {
    }

    // ========================================================================
    // Lifecycle Management
    // ========================================================================

    void Service::start() {
        // Validate transition: Only an 'Initialized' service can move to 'Running'.
        const bool canStart = m_state.read([this](const State& current) -> bool {
            if (current == State::Running)   return (m_logger->warn("Service '{}' is already running.", m_name), false);
            if (current == State::Shutdown)  return (m_logger->error("Cannot start service '{}': it has been shut down.", m_name), false);
            if (current == State::Paused)    return (m_logger->error("Service '{}' is paused; use resume() instead.", m_name), false);
            return current == State::Initialized;
        });

        if (!canStart) return;

        {
            // Atomically update state and trigger the user-defined startup hook.
            std::lock_guard<std::mutex> lock(m_mutex);
            m_state.write([this](State& s) {
                s = State::Running;
                onStart();
            });
            m_managedThread = true;
        }

        // Thread creation is performed outside the write-lock to ensure the
        // new thread doesn't immediately contend for the lock we just held.
        m_thread = std::thread(&Service::run, this);
    }

    void Service::stop() {
        const bool canStop = m_state.read([this](const State& current) -> bool {
            if (current == State::Shutdown) return (m_logger->warn("Service '{}' is already shut down.", m_name), false);
            return true;
        });

        if (!canStop) return;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_state.write([this](State& s) {
                s = State::Shutdown;
                onShutdown();
            });
            m_managedThread = false;
        }

        // Wake the thread if it's currently idling in a 'Paused' state.
        m_cv.notify_all();

        // Critical: Join outside of locks to prevent deadlocks where the
        // worker thread tries to acquire a lock the caller is still holding.
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    void Service::pause() {
        if (!m_managedThread) return;

        const bool canPause = m_state.read([this](const State& current) -> bool {
            if (current != State::Running) return (m_logger->error("Service '{}' must be running to be paused.", m_name), false);
            return true;
        });

        if (canPause) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_state.write([this](State& s) {
                s = State::Paused;
                onPause();
            });
        }
    }

    void Service::resume() {
        if (!m_managedThread) return;

        const bool canResume = m_state.read([this](const State& current) -> bool {
            if (current != State::Paused) return (m_logger->error("Service '{}' must be paused to be resumed.", m_name), false);
            return true;
        });

        if (canResume) {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_state.write([this](State& s) {
                    s = State::Running;
                    onResume();
                });
            }
            m_cv.notify_all(); // Wake the loop.
        }
    }

    // ========================================================================
    // Internal Execution
    // ========================================================================

    void Service::run() {
        // Push the service's logger onto the context stack for the duration of
        // the game loop so that any Logger::context() call in derived code resolves here.
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;

        m_state.write([this](State& s) { s = State::Running; });

        while (true) {
            const State current = m_state.read([](const State& s) { return s; });

            if (current == State::Shutdown) break;

            if (current == State::Paused) {
                std::unique_lock<std::mutex> pauseLock(m_mutex);
                m_cv.wait(pauseLock, [this] {
                    // Predicate: Wake up if we are no longer paused.
                    return m_state.read([](const State& s) { return s != State::Paused; });
                });
                continue; // Re-evaluate state immediately.
            }

            // Standard Execution Pulse
            if (current == State::Running) {
                onUpdate();
                onRender();
                onEvent();
            }
        }
    }

} // namespace Harmony