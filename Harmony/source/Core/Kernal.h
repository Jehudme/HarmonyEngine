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
        void finalize() override;

        virtual void update() override;
        virtual void render() override;
        virtual void event() override;

        Controller& controller(const std::string& type) override;


        flecs::world world;
    };
}
