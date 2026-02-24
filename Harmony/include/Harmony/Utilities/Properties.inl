#pragma once

namespace Harmony {

    template<typename Type>
    inline void Properties::set(const Path& key_path, const Type& value) {
        _set_raw(key_path, &value, typeid(Type));
    }

    template<typename Type>
    inline std::optional<Type> Properties::get(const Path& key_path) const {
        Type result;
        if (_get_raw(key_path, &result, typeid(Type))) {
            return result;
        }
        return std::nullopt;
    }

}