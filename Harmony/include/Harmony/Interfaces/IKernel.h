#pragma once
#include "Harmony/Utilities/Properties.h"
#include "Harmony/Core/Extension.h"

namespace Harmony
{
    using EntityId = uint32_t;

    class IKernel
    {
    public:
        explicit IKernel() = default;
        virtual ~IKernel() = default;

        virtual void initialize(const Properties& properties) = 0;

        virtual void progress() = 0;

        virtual Extension& extension(const std::string& type) = 0;

        // Pointer-returning API for graceful failure (returns nullptr if not found).
        Extension* getExtension(const std::string& type);

        template<typename ExtensionType>
        ExtensionType* getExtension();
    };
}

#include "Harmony/Interfaces/IKernel.inl"