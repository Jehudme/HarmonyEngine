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
        // 1. REMOVE the HARMONY_EXTENSION_IMPLEMTATION macro

        // 2. Manually add the name getters the macro would have made
        static const std::string& GET_NAME() { static std::string name = "kernel"; return name; }
        const std::string& getName() const override { return GET_NAME(); }

        explicit Kernel();
        ~Kernel() override;

        void initialize(const Properties& properties) override;
        void setupAutomation();

        void progress() override;
        Extension* extension(const std::string& type) override;

    private:
        flecs::world m_world;
        Logger m_logger;
    };
}