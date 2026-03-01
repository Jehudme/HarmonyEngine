#pragma once

#ifdef __GNUC__
#include <cxxabi.h>
#endif

namespace Harmony
{
    inline std::string demangle(const char* name) {
        #ifdef __GNUC__
        int status = -1;
        char* demangled = abi::__cxa_demangle(name, NULL, NULL, &status);
        std::string result = (status == 0) ? demangled : name;
        free(demangled);
        return result;
        #else
        return name;
        #endif
    }
    template<typename Base, typename Derived, typename ...Args>
    inline void Registry::save(const std::string& name)
    {
        static_assert(std::is_base_of_v<Base, Derived>, "Derived must inherit from Base");

        m_registry.write([&](RegistryMap& registry) {
            // Because the map is flat, names must be globally unique
            if (registry.contains(name)) {
                std::string message = std::format("Registry error: Factory name '{}' is already in use.", name);
                m_logger.error("{}", message);
                throw std::runtime_error(message);
            }

            using FactoryType = std::function<std::unique_ptr<Base>(Args...)>;
            registry[name] = FactoryType([](Args... args) -> std::unique_ptr<Base> {
                return std::make_unique<Derived>(std::forward<Args>(args)...);
            });

            m_logger.info("Saved factory '{}' for type '{}'.", name, demangle(typeid(Base).name()));
        });
    }

    // No longer a template function!
    inline void Registry::free(const std::string& name)
    {
        m_registry.write([&](RegistryMap& registry) {
            if (registry.erase(name) > 0) {
                m_logger.info("Freed factory '{}'.", name);
            } else {
                m_logger.warn("Free failed: Factory '{}' not found.", name);
            }
        });
    }

    template<typename Type, typename ...Args>
    inline std::unique_ptr<Type> Registry::create(const std::string& name, Args&& ...args)
    {
        return m_registry.read([&](const RegistryMap& registry) -> std::unique_ptr<Type> {
            auto factoryEntry = registry.find(name);
            if (factoryEntry == registry.end()) {
                m_logger.error("Create failed: Factory '{}' not found.", name);
                return nullptr;
            }

            using FactoryType = std::function<std::unique_ptr<Type>(Args...)>;

            try {
                // If the user requests wrong types or args, any_cast throws safely
                const auto& factory = std::any_cast<const FactoryType&>(factoryEntry->second);
                m_logger.trace("Creating instance '{}' for type '{}'.", name, typeid(Type).name());
                return factory(std::forward<Args>(args)...);
            }
            catch (const std::bad_any_cast&) {
                m_logger.error("Create failed: Type or argument mismatch for factory '{}'.", name);
                return nullptr;
            }
        });
    }
}