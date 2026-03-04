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
        static const std::string& GET_NAME() { static std::string name = "kernel"; return name; }
        [[nodiscard]] const std::string& getName() const override { return GET_NAME(); }

        explicit Kernel();
        ~Kernel() override;

        void initialize(const Properties& properties) override;

        void progress() override;
        Extension* extension(const std::string& type) override;

    private:
        void setupAutomation();

        flecs::world m_world;
        Logger m_logger;
    };
}