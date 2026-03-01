#pragma once

namespace Harmony {

    template<typename Type>
    inline void Properties::set(Path keyPath, const Type& value) {
        setRawValue(keyPath, &value, typeid(Type));
    }

    template<typename Type>
    inline std::optional<Type> Properties::get(Path keyPath) const {
        Type result;
        if (getRawValue(keyPath, &result, typeid(Type))) {
            return result;
        }
        return std::nullopt;
    }

}