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
        static void save(const std::string& name);

        static void free(const std::string& name);

        template<typename Type, typename ...Args>
        static std::unique_ptr<Type> create(const std::string& name, Args&& ...args);

    private:
        using RegistryMap = std::unordered_map<std::string, std::any>;

        static inline Guarded<RegistryMap> m_registry = Guarded<RegistryMap>();
        static inline Logger& m_logger = Logger::global();
    };
}

// --- Macros ---

// Automatic Registration (Runs before main)
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

// Manual Unregistration (Now only requires the string name!)
#define HARMONY_UNREGISTER_MANUAL(Name) \
    ::Harmony::Registry::free(Name)

#include "Registry.inl"
