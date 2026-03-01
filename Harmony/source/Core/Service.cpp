#include <Harmony/Core/Service.h>
#include "Harmony/Core/ContextLogger.h"
#include <chrono>

namespace Harmony {

    // Target maximum tick rate for the raw loop (1000 FPS cap) to prevent CPU pegging
    static constexpr float kMaxTickRate = 1000.0f;
    static constexpr float kMinFrameTime = 1.0f / kMaxTickRate;

    Service::Service(const std::string& name, const std::string& type, Engine& engine)
        : Extension(name, type, engine) {
    }

    // ========================================================================
    // Lifecycle Management
    // ========================================================================

    void Service::start() {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());

        // Validate transition: Only an 'Initialized' service can move to 'Running'.
        const State currentState = m_state.load(std::memory_order_acquire);
        
        if (currentState == State::Running) {
            m_logger->warn("Service '{}' of type '{}' is already running; ignoring start() call.", m_name, m_type);
            return;
        }
        if (currentState == State::Shutdown) {
            m_logger->error("Cannot start service '{}': it has been shut down permanently.", m_name);
            return;
        }
        if (currentState == State::Paused) {
            m_logger->error("Service '{}' is paused; use resume() to continue execution.", m_name);
            return;
        }
        if (currentState != State::Initialized) {
            return;
        }

        {
            // Atomically update state and trigger the user-defined startup hook.
            // Lock ensures no concurrent state transitions during initialization.
            std::lock_guard<std::mutex> lock(m_serviceMutex);
            m_state.store(State::Running, std::memory_order_release);
            onStart();
            m_isThreadManaged = true;
            m_logger->info("Service '{}' of type '{}' transitioning to Running state.", m_name, m_type);
        }

        // Thread creation occurs outside the lock to prevent the new thread
        // from immediately contending for the mutex we just released.
        m_thread = std::thread(&Service::run, this);
    }

    void Service::stop() {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());

        const State currentState = m_state.load(std::memory_order_acquire);
        if (currentState == State::Shutdown) {
            m_logger->warn("Service '{}' of type '{}' is already shut down.", m_name, m_type);
            return;
        }

        {
            std::lock_guard<std::mutex> lock(m_serviceMutex);
            m_state.store(State::Shutdown, std::memory_order_release);
            onShutdown();
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
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());

        if (!m_isThreadManaged) {
            m_logger->warn("Service '{}' has no managed thread; pause() has no effect.", m_name);
            return;
        }

        const State currentState = m_state.load(std::memory_order_acquire);
        if (currentState != State::Running) {
            m_logger->error("Service '{}' must be Running to be paused; current state does not allow pause.", m_name);
            return;
        }

        {
            std::lock_guard<std::mutex> lock(m_serviceMutex);
            m_state.store(State::Paused, std::memory_order_release);
            onPause();
            m_logger->info("Service '{}' of type '{}' transitioning to Paused state.", m_name, m_type);
        }
    }

    void Service::resume() {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());

        if (!m_isThreadManaged) {
            m_logger->warn("Service '{}' has no managed thread; resume() has no effect.", m_name);
            return;
        }

        const State currentState = m_state.load(std::memory_order_acquire);
        if (currentState != State::Paused) {
            m_logger->error("Service '{}' must be Paused to be resumed; current state does not allow resume.", m_name);
            return;
        }

        {
            std::lock_guard<std::mutex> lock(m_serviceMutex);
            m_state.store(State::Running, std::memory_order_release);
            onResume();
            m_logger->info("Service '{}' of type '{}' transitioning to Running state from Paused.", m_name, m_type);
        }
        // Wake the run loop so it can resume execution.
        m_conditionVariable.notify_all();
    }

    // ========================================================================
    // Internal Execution
    // ========================================================================

    void Service::run() {
        // Push the service's logger onto the thread-local context stack for the
        // duration of the run loop. Any nested Logger::context() calls resolve here.
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());

        m_state.store(State::Running, std::memory_order_release);
        m_logger->trace("Service '{}' run loop started on dedicated thread.", m_name);

        using Clock = std::chrono::high_resolution_clock;
        auto lastFrameTime = Clock::now();

        while (true) {
            // Capture frame start time at the very beginning of each iteration
            auto frameStartTime = Clock::now();
            
            const State currentState = m_state.load(std::memory_order_acquire);

            if (currentState == State::Shutdown) {
                m_logger->trace("Service '{}' detected Shutdown state; exiting run loop.", m_name);
                break;
            }

            if (currentState == State::Paused) {
                // Block until the state is no longer Paused. The condition variable
                // prevents busy-waiting while paused.
                std::unique_lock<std::mutex> pauseLock(m_serviceMutex);
                m_conditionVariable.wait(pauseLock, [this] {
                    return m_state.load(std::memory_order_acquire) != State::Paused;
                });
                // Reset timing after pause to avoid large delta spikes
                lastFrameTime = Clock::now();
                continue; // Re-evaluate state immediately after waking.
            }

            // Standard Execution Pulse: update, render, and process events.
            if (currentState == State::Running) {
                // Calculate delta time since last frame started
                std::chrono::duration<float> frameDuration = frameStartTime - lastFrameTime;
                float deltaTime = frameDuration.count();
                lastFrameTime = frameStartTime;

                onUpdate(deltaTime);
                onRender();
                onEvent();

                // CPU yielding: If frame completed faster than the minimum frame time,
                // yield to prevent pegging the CPU at 100% when VSync is off.
                auto frameEndTime = Clock::now();
                std::chrono::duration<float> frameProcessingTime = frameEndTime - frameStartTime;
                float processingTime = frameProcessingTime.count();
                
                if (processingTime < kMinFrameTime) {
                    std::this_thread::yield();
                }
            }
        }

        m_logger->trace("Service '{}' run loop has exited.", m_name);
    }

} // namespace Harmony