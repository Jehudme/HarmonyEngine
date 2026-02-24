#include <Harmony/Systems/Service.h>

namespace Harmony {
    Service::Service(Engine& engine) : Controller(engine) {
    }

    void Service::start() {
        if (state.read([this](const State& state) -> bool {
            switch (state) {
            case State::Running:
                logger->warn("Attempted to start service while it is already running.");
                return false;
            case State::Shutdown:
                logger->error("Attempted to start service after it was shutdown.");
                return false;
            case State::Paused:
                logger->error("Attempted to start service while it was paused.");
                return false;
            default:
                return true;
        }
        })) { return; }

        std::lock_guard<std::mutex> lock(m_mutex);
        state.write([this](State& state) -> void {
            state = State::Running;

            onStart();
            m_managedThread = true;
            m_thread = std::thread(&Service::run, this);
        });
    }

    void Service::stop() {
        if (!m_managedThread) return logger->error("Attempted to stop service that was not started with a thread.");

        std::lock_guard<std::mutex> lock(m_mutex);
        state.write([this](State& state) -> void {
            state = State::Shutdown;

            m_thread.join();
            onShutdown();
            m_managedThread = false;
        });
    }

    void Service::resume() {
        if (!m_managedThread) return logger->error("Attempted to resume service that was not started with a thread.");

        std::lock_guard<std::mutex> lock(m_mutex);
        state.write([this](State& state) -> void {
            state = State::Running;

            onResume();
            m_cv.notify_one();
        });

    }

    void Service::pause() {
        if (!m_managedThread) return logger->error("Attempted to pause service that was not started with a thread.");

        std::lock_guard<std::mutex> lock(m_mutex);
        state.write([this](State& state) -> void {
            state = State::Paused;

            onPause();
        });
    }

    void Service::run() {
        while (state.read([](const State& state) -> bool { return state != State::Shutdown; })) {
            if (state.read([](const State& state) -> bool { return state == State::Paused; })) {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cv.wait(lock, [this]() -> bool {
                    return state.read([](const State& state) -> bool { return state != State::Paused; });
                });
            }

            state.read([this](const State& state) -> void {
                if (state != State::Running) return;
                onUpdate();
                onRender();
                onEvent();
             });
        }
    }
}
