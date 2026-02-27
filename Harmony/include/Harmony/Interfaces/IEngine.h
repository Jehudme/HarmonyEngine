#pragma once
#include <Harmony/Interfaces/IWindow.h>

#include "Harmony/Core/Service.h"


namespace Harmony
{
    class IEngine : public Service, private Uncopyable
    {
        IEngine() : Service(*this) {};

        IEngine& getInterface() { return *this; }

        public:
        virtual IWindow& getWindow() = 0;


    };
}

