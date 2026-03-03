#pragma once
#include "Harmony/Utilities/Properties.h"
#include "Harmony/Core/Extension.h"

namespace Harmony
{
    using EntityId = uint32_t;

    class IKernel : public Extension
    {
    public:
        HARMONY_EXTENSION_INTERFACE(IKernel, "kernal");

        virtual void initialize(const Properties& properties) = 0;

        virtual void progress() = 0;

        virtual Extension* extension(const std::string& type) = 0;

        template<typename ExtensionType>
        ExtensionType* extension();
    };
}

#include "Harmony/Interfaces/IKernel.inl"