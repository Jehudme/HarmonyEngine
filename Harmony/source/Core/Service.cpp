#include <Harmony/Core/Service.h>

namespace Harmony {

    Service::Service(IEngine& engine) : Controller(engine) {
    }

    // ========================================================================
    // Lifecycle Management
    // ========================================================================

    void Service::start() {
        // Validate transition: Only an 'Initialized' service can move to 'Running'.
        const bool canStart = state.read([this](const State& current) -> bool {
            if (current == State::Running)   return (logger->warn("Service is already running."), false);
            if (current == State::Shutdown)  return (logger->error("Cannot start a shutdown service."), false);
            if (current == State::Paused)    return (logger->error("Service is paused; use resume() instead."), false);
            return current == State::Initialized;
        });

        if (!canStart) return;

        {
            // Atomically update state and trigger the user-defined startup hook.
            std::lock_guard<std::mutex> lock(m_mutex);
            state.write([this](State& s) {
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
        if (!m_managedThread) {
            logger->error("Stop failed: Service does not possess a managed worker thread.");
            return;
        }

        const bool canStop = state.read([this](const State& current) -> bool {
            if (current == State::Shutdown) return (logger->warn("Service already shutdown."), false);
            return true;
        });

        if (!canStop) return;

        {
            // Transition to Shutdown; this will cause the 'run' loop to break.
            std::lock_guard<std::mutex> lock(m_mutex);
            state.write([this](State& state) {
                state = State::Shutdown;
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

        const bool canPause = state.read([this](const State& current) -> bool {
            if (current != State::Running) return (logger->error("Only running services can be paused."), false);
            return true;
        });

        if (canPause) {
            std::lock_guard<std::mutex> lock(m_mutex);
            state.write([this](State& state) {
                state = State::Paused;
                onPause();
            });
        }
    }

    void Service::resume() {
        if (!m_managedThread) return;

        const bool canResume = state.read([this](const State& current) -> bool {
            if (current != State::Paused) return (logger->error("Only paused services can be resumed."), false);
            return true;
        });

        if (canResume) {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                state.write([this](State& state) {
                    state = State::Running;
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
        /*
         * Main Execution Loop
         * * This loop utilizes a hybrid locking strategy:
         * 1. A shared-lock (Read) to check if we should continue running.
         * 2. A unique-lock (Mutex) combined with a condition variable for
         * low-overhead idling when the service is paused.
         */
        while (true) {
            const State current = state.read([](const State& s) { return s; });

            if (current == State::Shutdown) break;

            if (current == State::Paused) {
                std::unique_lock<std::mutex> pauseLock(m_mutex);
                m_cv.wait(pauseLock, [this] {
                    // Predicate: Wake up if we are no longer paused.
                    return state.read([](const State& s) { return s != State::Paused; });
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