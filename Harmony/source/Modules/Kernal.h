#pragma once
#include "Harmony/Interfaces/IKernel.h"
#include "Harmony/Utilities/Properties.h"

namespace flecs {
    struct world;
}

namespace Harmony
{
    class Kernal : public IKernel {
        public:
        Kernal();
        ~Kernal() override;

        void initialize(const Properties& properties) override;

        flecs::world world;
    };
}
