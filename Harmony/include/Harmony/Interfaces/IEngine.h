#pragma once
#include <Harmony/Interfaces/IWindow.h>


namespace Harmony
{
    class IEngine
    {
        public:
        IEngine() = default;
        virtual ~IEngine() = default;

        virtual IWindow& getWindow() = 0;
    };
}

