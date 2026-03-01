#pragma once

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <any>
#include <stdexcept>
#include <format>

#include <Harmony/Utilities/Guarded.h>
#include <Harmony/Core/Logger.h>

namespace Harmony
{
    // Registry provides a thread-safe type-erased factory system for dynamic object creation.
    // Types are registered with a string name and created via type-erased factory functions.
    // This enables runtime-configurable extension instantiation from JSON configuration.
    class Registry
    {
    public:
        Registry() = delete;

        Registry(const Registry&) = delete;
        Registry& operator=(const Registry&) = delete;
        Registry(Registry&&) = delete;
        Registry& operator=(Registry&&) = delete;

        // Registers a factory function that creates Derived instances as Base pointers.
        // The factory accepts Args... and returns std::unique_ptr<Base>.
        template<typename Base, typename Derived, typename ...Args>
        static void save(const std::string& factoryName);

        // Removes a factory from the registry by name.
        static void free(const std::string& factoryName);

        // Creates an instance using the registered factory, returning nullptr on failure.
        // Type mismatch between stored and requested types is handled gracefully.
        template<typename Type, typename ...Args>
        static std::unique_ptr<Type> create(const std::string& factoryName, Args&& ...args);

    private:
        // Type-erased map: factory name -> std::any holding std::function<std::unique_ptr<T>(Args...)>
        using RegistryMap = std::unordered_map<std::string, std::any>;

        static inline Guarded<RegistryMap> m_registry = Guarded<RegistryMap>();
        static inline Logger& m_logger = Logger::global();
    };
}

// --- Macros ---

// Automatic Registration (Runs before main via static initialization)
#define HARMONY_REGISTER(Base, Derived, Name, ...)                         \
    namespace {                                                            \
        struct HarmonyRegistrar_##Derived##_##__LINE__ {                   \
            HarmonyRegistrar_##Derived##_##__LINE__() {                    \
                ::Harmony::Registry::save<Base, Derived, ##__VA_ARGS__>(Name); \
            }                                                              \
            ~HarmonyRegistrar_##Derived##_##__LINE__() {                   \
                ::Harmony::Registry::free(Name);                           \
            }                                                              \
        };                                                                 \
        static inline HarmonyRegistrar_##Derived##_##__LINE__ global_reg_##Derived##_##__LINE__; \
    }

// Manual Registration
#define HARMONY_REGISTER_MANUAL(Base, Derived, Name, ...) \
    ::Harmony::Registry::save<Base, Derived, ##__VA_ARGS__>(Name)

// Manual Unregistration (Requires only the string name)
#define HARMONY_UNREGISTER_MANUAL(Name) \
    ::Harmony::Registry::free(Name)

#include "Registry.inl"
