#pragma once

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <any>
#include <tuple>
#include <stdexcept>
#include <format>

#include <Harmony/Utilities/Guarded.h>
#include <Harmony/Utilities/Logger.h>

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

        template<typename Base, typename ...Args>
        static void free(const std::string& name);

        template<typename Type, typename ...Args>
        static std::unique_ptr<Type> create(const std::string& name, Args&& ...args);

    private:
        using TypeKey = std::type_index;
        using ArgsKey = std::type_index;
        using FactoryMap = std::unordered_map<std::string, std::any>;
        using RegistryMap = std::unordered_map<TypeKey, std::unordered_map<ArgsKey, FactoryMap>>;

        static inline Guarded<RegistryMap> m_registry = Guarded<RegistryMap>();
        static inline Logger& m_logger = Logger::globalInstance();
    };
}

#include "Harmony/Systems/Registry.inl"