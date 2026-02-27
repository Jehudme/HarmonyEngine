#pragma once
#include <condition_variable>
#include <thread>
#include <Harmony/Core/Controller.h>

namespace Harmony {
    class IEngine;

    class Service : public Controller
    {
    public:
        explicit Service(IEngine& engine);
        ~Service() override = default;

        void start();
        void stop();
        void resume();
        void pause();

        void run();

    protected:
        virtual void onStart() = 0;
        virtual void onShutdown() = 0;
        virtual void onPause() = 0;
        virtual void onResume() = 0;

    private:
        std::mutex m_mutex;
        std::thread m_thread;
        std::condition_variable m_cv;
        bool m_managedThread = false;

    };

}
