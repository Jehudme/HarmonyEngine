#pragma once
#include "Harmony/Interfaces/IKernel.h"
#include "Harmony/Utilities/Properties.h"

#include <memory>

namespace flecs {
    struct world;
}

namespace Harmony
{
    class Kernel : public IKernel {
        public:
        Kernel();
        ~Kernel() override;

        void initialize(Engine& engine, const Properties& properties) override;
        void finalize() override;

        void update(float deltaTime) override;
        void render() override;
        void event() override;

        Extension& extension(const std::string& type) override;

    private:
        std::unique_ptr<flecs::world> m_world;
    };
}
