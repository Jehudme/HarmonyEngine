#pragma once

namespace Harmony
{
    template<typename ExtensionType>
    ExtensionType* IKernel::extension() {
        static_assert(std::is_base_of_v<Extension, ExtensionType>, "T must derive from Extension to be used in getExtension<T>()");
        Extension* ext = extension(std::string(ExtensionType::GET_TYPE()));
        return ext ? static_cast<ExtensionType*>(ext) : nullptr;
    }
}