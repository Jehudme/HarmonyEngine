#pragma once
#include "Harmony/Interfaces/IRunner.h"

namespace Harmony
{
    class Runner : public IRunner
    {
    public:
        HARMONY_EXTENSION_IMPLEMTATION(IRunner, Runner, "runner")

        void start() override;
        void run() override;
        void stop() override;
        void pause() override;
        void resume() override;
    };
} // Harmony
