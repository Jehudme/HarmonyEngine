#pragma once
#include "Harmony/Utilities/Properties.h"
#include "Harmony/Core/Extension.h"

namespace Harmony
{
    class IKernel
    {
    public:
        explicit IKernel() = default;
        virtual ~IKernel() = default;

        virtual void initialize(Engine& engine, const Properties& properties) = 0;
        virtual void finalize() = 0;

        virtual void update(float deltaTime) = 0;
        virtual void render() = 0;
        virtual void event() = 0;

        virtual Extension& extension(const std::string& type) = 0;

        template<typename ExtensionType>
        ExtensionType& extension()
        {
            static_assert(std::is_base_of_v<Extension, ExtensionType>,
                  "T must derive from Extension to be used in extension<T>()");

            return extension(ExtensionType::getType());
        }
    };

};
