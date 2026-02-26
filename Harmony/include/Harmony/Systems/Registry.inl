#pragma once

namespace Harmony
{
    template<typename Base, typename Derived, typename ...Args>
    inline void Registry::save(const std::string& name)
    {
        static_assert(std::is_base_of_v<Base, Derived>);

        m_registry.write([&](RegistryMap& registry) {
            auto& factories = registry[typeid(Base)][typeid(std::tuple<Args...>)];

            if (factories.contains(name)) {
                std::string message = std::format("Registry error: Name '{}' is already saved for type '{}'.", name, typeid(Base).name());
                m_logger.error("{}", message);
                throw std::runtime_error(message);
            }

            factories[name] = std::function<std::unique_ptr<Base>(Args...)>([](Args... args) -> std::unique_ptr<Base> {
                return std::make_unique<Derived>(std::forward<Args>(args)...);
            });

            m_logger.info("Saved factory '{}' for type '{}'.", name, typeid(Base).name());
        });
    }

    template<typename Base, typename ...Args>
    inline void Registry::free(const std::string& name)
    {
        m_registry.write([&](RegistryMap& registry) {
            auto typeIterator = registry.find(typeid(Base));
            if (typeIterator == registry.end()) {
                m_logger.warn("Free failed: Type '{}' not found.", typeid(Base).name());
                return;
            }

            auto argsIterator = typeIterator->second.find(typeid(std::tuple<Args...>));
            if (argsIterator == typeIterator->second.end()) {
                m_logger.warn("Free failed: Arguments for type '{}' not found.", typeid(Base).name());
                return;
            }

            if (argsIterator->second.erase(name) > 0) {
                m_logger.info("Freed factory '{}' for type '{}'.", name, typeid(Base).name());
            } else {
                m_logger.warn("Free failed: Factory '{}' not found for type '{}'.", name, typeid(Base).name());
            }
        });
    }

    template<typename Type, typename ...Args>
    inline std::unique_ptr<Type> Registry::create(const std::string& name, Args&& ...args)
    {
        return m_registry.read([&](const RegistryMap& registry) -> std::unique_ptr<Type> {
            auto typeIterator = registry.find(typeid(Type));
            if (typeIterator == registry.end()) {
                m_logger.error("Create failed: Type '{}' is not registered.", typeid(Type).name());
                return nullptr;
            }

            auto argsIterator = typeIterator->second.find(typeid(std::tuple<std::decay_t<Args>...>));
            if (argsIterator == typeIterator->second.end()) {
                m_logger.error("Create failed: No matching arguments for type '{}'.", typeid(Type).name());
                return nullptr;
            }

            auto factoryEntry = argsIterator->second.find(name);
            if (factoryEntry == argsIterator->second.end()) {
                m_logger.error("Create failed: Factory '{}' not found for type '{}'.", name, typeid(Type).name());
                return nullptr;
            }

            using FactoryType = std::function<std::unique_ptr<Type>(std::decay_t<Args>...)>;
            const auto& factory = std::any_cast<const FactoryType&>(factoryEntry->second);

            m_logger.trace("Creating instance '{}' for type '{}'.", name, typeid(Type).name());
            return factory(std::forward<Args>(args)...);
        });
    }
}