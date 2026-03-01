#pragma once

#include <mutex>
#include <shared_mutex>
#include <utility>

namespace Harmony {

    template<typename ValueType>
    template<typename ...Args>
    inline Guarded<ValueType>::Guarded(Args&& ...args)
        : m_value(std::make_unique<ValueType>(std::forward<Args>(args)...)) {
    }

    template<typename ValueType>
    inline Guarded<ValueType>::Guarded(const Guarded& other) {
        // Must acquire read lock on 'other' to safely copy its value.
        other.read([this](const ValueType& otherValue) {
            m_value = std::make_unique<ValueType>(otherValue);
        });
    }

    template<typename ValueType>
    inline Guarded<ValueType>& Guarded<ValueType>::operator=(const Guarded& other) {
        if (this != &other) {
            // Nested locking: acquire write lock on 'this', then read lock on 'other'.
            // Lock ordering is consistent to prevent deadlock in symmetric assignments.
            this->write([&other](ValueType& thisValue) {
                other.read([&thisValue](const ValueType& otherValue) {
                    thisValue = otherValue;
                });
            });
        }
        return *this;
    }

    template<typename ValueType>
    inline Guarded<ValueType>& Guarded<ValueType>::operator=(const ValueType& other) {
        this->write([&other](ValueType& thisValue) {
            thisValue = other;
        });
        return *this;
    }

    template<typename ValueType>
    inline Guarded<ValueType>::Guarded(Guarded&& other) noexcept {
        // Move construction: take exclusive ownership of 'other's value.
        other.write([this](ValueType& otherValue) {
            m_value = std::make_unique<ValueType>(std::move(otherValue));
        });
    }

    template<typename ValueType>
    inline Guarded<ValueType>& Guarded<ValueType>::operator=(Guarded&& other) noexcept {
        if (this != &other) {
            // Move assignment: acquire exclusive locks on both objects.
            this->write([&other](ValueType& thisValue) {
                other.write([&thisValue](ValueType& otherValue) {
                    thisValue = std::move(otherValue);
                });
            });
        }
        return *this;
    }

    template<typename ValueType>
    inline Guarded<ValueType>& Guarded<ValueType>::operator=(ValueType&& other) noexcept {
        this->write([&other](ValueType& thisValue) {
            thisValue = std::move(other);
        });
        return *this;
    }

    template<typename ValueType>
    template<typename FunctionType>
    auto Guarded<ValueType>::read(FunctionType function) const -> decltype(function(std::declval<const ValueType&>())) {
        // shared_lock permits multiple concurrent readers while blocking writers.
        std::shared_lock lock(m_mutex);
        return function(*m_value);
    }

    template<typename ValueType>
    template<typename FunctionType>
    auto Guarded<ValueType>::write(FunctionType function) -> decltype(function(std::declval<ValueType&>())) {
        // scoped_lock acquires exclusive ownership; all readers and writers are blocked.
        std::scoped_lock lock(m_mutex);
        return function(*m_value);
    }

}