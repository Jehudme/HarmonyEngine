#pragma once

#include <shared_mutex>
#include <utility>
#include <memory>
#include <type_traits>

namespace Harmony {

    template<typename ValueType>
    class Guarded {
    public:
        template<typename ...Args>
        explicit Guarded(Args&& ...args);
        ~Guarded() = default;

        Guarded(const Guarded& other);
        Guarded& operator=(const Guarded& other);
        Guarded& operator=(const ValueType& other);


        Guarded(Guarded&& other) noexcept;
        Guarded& operator=(Guarded&& other) noexcept;
        Guarded& operator=(ValueType&& other) noexcept;

        template<typename FunctionType>
        auto read(FunctionType function) const -> decltype(function(std::declval<const ValueType&>()));

        template<typename FunctionType>
        auto write(FunctionType function) -> decltype(function(std::declval<ValueType&>()));

    private:
        mutable std::shared_mutex m_mutex;
        std::unique_ptr<ValueType> m_value;
    };
}

#include "Harmony/Utilities/Guarded.inl"