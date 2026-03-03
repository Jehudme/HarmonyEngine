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
    class Registry
    {
    public:
        Registry() = delete;

        Registry(const Registry&) = delete;
        Registry& operator=(const Registry&) = delete;
        Registry(Registry&&) = delete;
        Registry& operator=(Registry&&) = delete;

        template<typename Base, typename Derived, typename ...Args>
        static void save(const std::string& factoryName);

        static void free(const std::string& factoryName);

        static bool exists(const std::string& factoryName);

        template<typename Type, typename ...Args>
        static std::unique_ptr<Type> create(const std::string& factoryName, Args&& ...args);

    private:
        // Type-erased map: factory name -> std::any holding std::function<std::unique_ptr<T>(Args...)>
        using RegistryMap = std::unordered_map<std::string, std::any>;

        static inline Guarded<RegistryMap> m_registry = Guarded<RegistryMap>();
        static inline Logger m_logger{"Registry"};
    };
}

// --- Macros ---

// Helper macros for token pasting with __COUNTER__
#define HARMONY_CONCAT_IMPL(a, b) a##b
#define HARMONY_CONCAT(a, b) HARMONY_CONCAT_IMPL(a, b)

// Automatic Registration (Runs before main via static initialization)
// Uses __COUNTER__ combined with Derived to guarantee unique struct names in unity builds.
#define HARMONY_REGISTER_IMPL(Base, Derived, Name, Counter, ...)                    \
    namespace {                                                                     \
        struct HARMONY_CONCAT(HarmonyRegistrar_##Derived##_, Counter) {             \
            HARMONY_CONCAT(HarmonyRegistrar_##Derived##_, Counter)() {              \
                ::Harmony::Registry::save<Base, Derived, ##__VA_ARGS__>(Name);      \
            }                                                                       \
            ~HARMONY_CONCAT(HarmonyRegistrar_##Derived##_, Counter)() {             \
                ::Harmony::Registry::free(Name);                                    \
            }                                                                       \
        };                                                                          \
        static inline HARMONY_CONCAT(HarmonyRegistrar_##Derived##_, Counter)        \
            HARMONY_CONCAT(global_reg_##Derived##_, Counter);                       \
    }

#define HARMONY_REGISTER(Base, Derived, Name, ...)                                  \
    HARMONY_REGISTER_IMPL(Base, Derived, Name, __COUNTER__, ##__VA_ARGS__)

// Manual Registration
#define HARMONY_REGISTER_MANUAL(Base, Derived, Name, ...) \
    ::Harmony::Registry::save<Base, Derived, ##__VA_ARGS__>(Name)

// Manual Unregistration (Requires only the string name)
#define HARMONY_UNREGISTER_MANUAL(Name) \
    ::Harmony::Registry::free(Name)

#include "Registry.inl"
