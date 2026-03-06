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
        static const std::string& GET_NAME() { static std::string name = "default-kernel"; return name; }
        [[nodiscard]] const std::string& getName() const override { return GET_NAME(); }

        explicit Kernel();
        ~Kernel() override;

        void initialize(const Properties& properties) override;
        void finalize() override;

        void createScene(const std::string& name) override;
        void deleteScene(const std::string& name) override;

        void update() override;
        Extension* getExtension(const std::string& type) override;

        flecs::world m_world;
    };
}