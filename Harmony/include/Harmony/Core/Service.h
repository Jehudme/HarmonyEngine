#pragma once
#include <condition_variable>
#include <thread>
#include <Harmony/Core/Extension.h>

namespace Harmony {
    class Engine;

    // Service is an Extension that runs its own managed thread for asynchronous
    // lifecycle execution. It provides start/stop/pause/resume semantics and
    // drives the onUpdate/onRender/onEvent loop on a dedicated thread.
    class Service : public Extension
    {
    public:
        explicit Service(const std::string& name, const std::string& type, Engine& engine);
        ~Service() override = default;

        // Starts the service thread and transitions to Running state.
        void start();

        // Signals shutdown and joins the service thread.
        void stop();

        // Pauses the service loop; thread blocks until resumed.
        void pause();

        // Resumes a paused service.
        void resume();

        // Main run loop executed on the service thread.
        void run();

    protected:
        virtual void onStart() = 0;
        virtual void onShutdown() = 0;
        virtual void onPause() = 0;
        virtual void onResume() = 0;

    private:
        // Protects state transitions and lifecycle callbacks.
        std::mutex m_serviceMutex;

        // The managed thread that runs the service loop.
        std::thread m_thread;

        // Condition variable for pause/resume synchronization.
        std::condition_variable m_conditionVariable;

        // Indicates whether this service owns and manages a running thread.
        bool m_isThreadManaged = false;
    };
}
