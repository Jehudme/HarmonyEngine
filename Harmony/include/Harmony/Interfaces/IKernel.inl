#pragma once

namespace Harmony
{
    inline Extension* IKernel::getExtension(const std::string& type) {
        return &extension(type);
    }

    template<typename ExtensionType>
    ExtensionType* IKernel::getExtension() {
        static_assert(std::is_base_of_v<Extension, ExtensionType>, "T must derive from Extension to be used in getExtension<T>()");
        Extension* ext = getExtension(std::string(ExtensionType::EXTENSION_TYPE));
        return ext ? static_cast<ExtensionType*>(ext) : nullptr;
    }
}