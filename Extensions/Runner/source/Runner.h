#pragma once
#include "Harmony/Interfaces/IRunner.h"
#include <thread>

namespace Harmony
{
    class Runner : public IRunner
    {
    public:
        HARMONY_EXTENSION_IMPLEMTATION(IRunner, Runner, "default-runner")

        ~Runner() override;

        void start() override;
        void run() override;
        void stop() override;
        void pause() override;
        void resume() override;

    protected:
        // Pure virtual overrides required by the Extension base class
        void onInitialize(const Properties& properties) override;
        void onFinalize() override;
        void onUpdate() override;
        void onRender() override;
        void onEvent() override;

    private:
        std::thread m_workerThread;
    };
} // Harmony