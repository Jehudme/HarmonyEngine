#pragma once
#include "Harmony/Core/Extension.h"

namespace Harmony
{
    class IRunner : public Extension
    {
    public:
        HARMONY_EXTENSION_INTERFACE(IRunner, "runner");

        virtual void start() = 0;
        virtual void run() = 0;

        virtual void stop() = 0;
        virtual void pause() = 0;
        virtual void resume() = 0;
    };
}