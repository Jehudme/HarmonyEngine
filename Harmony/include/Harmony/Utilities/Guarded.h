#pragma once

#include <shared_mutex>
#include <utility>
#include <memory>
#include <type_traits>

namespace Harmony {

    // Guarded<T> provides thread-safe access to a value using reader-writer lock semantics.
    // Multiple concurrent readers are allowed, but writers have exclusive access.
    // This is ideal for scenarios where reads vastly outnumber writes.
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

        // Executes the provided function with read-only access to the guarded value.
        // Multiple readers can execute concurrently.
        template<typename FunctionType>
        auto read(FunctionType function) const -> decltype(function(std::declval<const ValueType&>()));

        // Executes the provided function with exclusive write access to the guarded value.
        // Blocks all other readers and writers until complete.
        template<typename FunctionType>
        auto write(FunctionType function) -> decltype(function(std::declval<ValueType&>()));

    private:
        // shared_mutex allows multiple shared (read) locks or one exclusive (write) lock.
        mutable std::shared_mutex m_mutex;

        // The guarded value is heap-allocated to allow move operations without invalidation.
        std::unique_ptr<ValueType> m_value;
    };
}

#include "Harmony/Utilities/Guarded.inl"