#include "flecs.h"
#include "Harmony/Core/Kernal.h"

#include "Harmony/Core/Controller.h"
#include "Harmony/Core/Engine.h"
#include "../../include/Harmony/Core/Logger.h"
#include "Harmony/Core/Registry.h"

namespace Harmony
{
    Harmony::Kernal::Kernal() : m_world(std::make_unique<flecs::world>()) {}
    Harmony::Kernal::~Kernal() = default;

    void Kernal::initialize(Engine& engine, const Properties& properties)
    {
        flecs::entity modules = m_world->entity("Kernel::Modules");
        properties.foreach({"modules"}, [&](const std::string& module_name, const Properties& module_properties)
        {
            if (std::unique_ptr<Controller> instance = Registry::create<Controller>(module_name, engine)) {
                const std::string ctrl_type = instance->getType();
                flecs::entity module = m_world->entity(ctrl_type.c_str());
                module.add(flecs::Module);
                modules.add(module);

                module.set<std::unique_ptr<Controller>>(std::move(instance));
                controller(ctrl_type).initialize(module_properties);
            }

            else Logger::global().critical("Harmony::Kernel::initialize - Failed to create module instance for '{}'", module_name);
        });
    }

    void Kernal::finalize()
    {
    m_world->each<std::unique_ptr<Controller>>([](std::unique_ptr<Controller>&  instance) {
            if (instance) return instance->finalize();
            Logger::global().critical("Harmony::Kernel::finalize - Failed to finalize module instance");
        });
    }

    void Kernal::update()
    {
        m_world->each<std::unique_ptr<Controller>>([](std::unique_ptr<Controller>& instance) {
            if (instance) return instance->update();
            Logger::global().critical("Harmony::Kernel::update - Failed to update module instance");
        });
    }

    void Kernal::render()
    {
        m_world->each<std::unique_ptr<Controller>>([](std::unique_ptr<Controller>& instance) {
            if (instance) return instance->render();
            Logger::global().critical("Harmony::Kernel::render - Failed to render module instance");
        });
    }

    void Kernal::event()
    {
        m_world->each<std::unique_ptr<Controller>>([](std::unique_ptr<Controller>& instance) {
            if (instance) return instance->event();
            Logger::global().critical("Harmony::Kernel::event - Failed to process event for module instance");
        });
    }

    Controller& Kernal::controller(const std::string& type) {
        return *m_world->entity(type.c_str()).get<std::unique_ptr<Controller>>();
    }

}
