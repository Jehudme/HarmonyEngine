#include "flecs.h"
#include "Harmony/Core/Kernel.h"

#include "Harmony/Core/Extension.h"
#include "Harmony/Core/Engine.h"
#include "Harmony/Core/Logger.h"
#include "Harmony/Core/Registry.h"

namespace Harmony
{
    Harmony::Kernel::Kernel() : m_world(std::make_unique<flecs::world>()) {}
    Harmony::Kernel::~Kernel() = default;

    void Kernel::initialize(Engine& engine, const Properties& properties)
    {
        flecs::entity modules = m_world->entity("Kernel::Modules");
        properties.foreach({"modules"}, [&](const std::string& moduleName, const Properties& moduleProperties)
        {
            if (std::unique_ptr<Extension> instance = Registry::create<Extension>(moduleName, engine)) {
                const std::string extensionType = instance->getType();
                flecs::entity module = m_world->entity(extensionType.c_str());
                module.add(flecs::Module);
                modules.add(module);

                module.set<std::unique_ptr<Extension>>(std::move(instance));
                extension(extensionType).initialize(moduleProperties);
            }

            else Logger::global().critical("Harmony::Kernel::initialize - Failed to create extension instance for '{}'", moduleName);
        });
    }

    void Kernel::finalize()
    {
        m_world->each<std::unique_ptr<Extension>>([](std::unique_ptr<Extension>& instance) {
            if (instance) return instance->finalize();
            Logger::global().critical("Harmony::Kernel::finalize - Failed to finalize extension instance");
        });
    }

    void Kernel::update()
    {
        m_world->each<std::unique_ptr<Extension>>([](std::unique_ptr<Extension>& instance) {
            if (instance) return instance->update();
            Logger::global().critical("Harmony::Kernel::update - Failed to update extension instance");
        });
    }

    void Kernel::render()
    {
        m_world->each<std::unique_ptr<Extension>>([](std::unique_ptr<Extension>& instance) {
            if (instance) return instance->render();
            Logger::global().critical("Harmony::Kernel::render - Failed to render extension instance");
        });
    }

    void Kernel::event()
    {
        m_world->each<std::unique_ptr<Extension>>([](std::unique_ptr<Extension>& instance) {
            if (instance) return instance->event();
            Logger::global().critical("Harmony::Kernel::event - Failed to process event for extension instance");
        });
    }

    Extension& Kernel::extension(const std::string& type) {
        return *m_world->entity(type.c_str()).get<std::unique_ptr<Extension>>();
    }

}
