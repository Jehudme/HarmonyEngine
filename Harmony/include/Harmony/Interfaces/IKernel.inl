#pragma once

namespace Harmony
{
    template<typename ExtensionType>
    ExtensionType& extension() {
        static_assert(std::is_base_of_v<Extension, ExtensionType>, "T must derive from Extension to be used in extension<T>()");
        return extension(ExtensionType::getType());
    }
}