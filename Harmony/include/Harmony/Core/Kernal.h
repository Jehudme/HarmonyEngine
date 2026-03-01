#pragma once
#include "Harmony/Interfaces/IKernel.h"
#include "Harmony/Utilities/Properties.h"

#include <memory>

namespace flecs {
    struct world;
}

namespace Harmony
{
    class Kernal : public IKernel {
        public:
        Kernal();
        ~Kernal() override;

        void initialize(Engine& engine, const Properties& properties) override;
        void finalize() override;

        virtual void update() override;
        virtual void render() override;
        virtual void event() override;

        Controller& controller(const std::string& type) override;

    private:
        std::unique_ptr<flecs::world> m_world;
    };
}
