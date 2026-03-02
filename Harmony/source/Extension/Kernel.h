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

        void initialize(const Properties& properties) override;
        void setupAutomation();

        void progress() override;
        Extension& extension(const std::string& type) override;

    private:
        std::unique_ptr<flecs::world> m_world;
        std::unique_ptr<Logger> m_logger;
    };
}
