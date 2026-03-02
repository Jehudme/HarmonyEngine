#pragma once

#ifdef __GNUC__
#include <cxxabi.h>
#endif
#include "ContextLogger.h"

namespace Harmony
{
    inline std::string demangle(const char* mangledName) {
        #ifdef __GNUC__
        int status = -1;
        char* demangled = abi::__cxa_demangle(mangledName, nullptr, nullptr, &status);
        std::string result = (status == 0) ? demangled : mangledName;
        free(demangled);
        return result;
        #else
        return mangledName;
        #endif
    }

    template<typename Base, typename Derived, typename ...Args>
    inline void Registry::save(const std::string& factoryName)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(&m_logger);

        static_assert(std::is_base_of_v<Base, Derived>, "Derived must inherit from Base");

        m_registry.write([&](RegistryMap& registry) {
            if (registry.contains(factoryName)) {
                const std::string errorMessage = std::format(
                    "Registry::save - Factory name '{}' is already registered; duplicate registration rejected.", factoryName);
                m_logger.error("{}", errorMessage);
                throw std::runtime_error(errorMessage);
            }

            using FactoryType = std::function<std::unique_ptr<Base>(Args...)>;
            registry[factoryName] = FactoryType([](Args... args) -> std::unique_ptr<Base> {
                return std::make_unique<Derived>(std::forward<Args>(args)...);
            });

            m_logger.info("Registry: Saved factory '{}' for base type '{}'.", factoryName, demangle(typeid(Base).name()));
        });
    }

    inline void Registry::free(const std::string& factoryName)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(&m_logger);

        m_registry.write([&](RegistryMap& registry) {
            if (registry.erase(factoryName) > 0) {
                m_logger.info("Registry: Freed factory '{}'.", factoryName);
            } else {
                m_logger.warn("Registry::free - Factory '{}' not found; nothing to remove.", factoryName);
            }
        });
    }

    template<typename Type, typename ...Args>
    inline std::unique_ptr<Type> Registry::create(const std::string& factoryName, Args&& ...args)
    {
        return m_registry.read([&](const RegistryMap& registry) -> std::unique_ptr<Type> {
            auto factoryIterator = registry.find(factoryName);
            if (factoryIterator == registry.end()) {
                m_logger.error("Registry::create - Factory '{}' not found in registry.", factoryName);
                return nullptr;
            }

            using FactoryType = std::function<std::unique_ptr<Type>(Args...)>;

            try {
                const auto& factory = std::any_cast<const FactoryType&>(factoryIterator->second);
                m_logger.trace("Registry: Creating instance '{}' of type '{}'.", factoryName, demangle(typeid(Type).name()));
                return factory(std::forward<Args>(args)...);
            }
            catch (const std::bad_any_cast&) {
                m_logger.error("Registry::create - Type or argument signature mismatch for factory '{}'. "
                              "Ensure the requested type matches the registered base type and arguments.", factoryName);
                return nullptr;
            }
        });
    }
}
