#include "flecs.h"
#include "Kernal.h"

#include "Harmony/Core/Controller.h"
#include "Harmony/Utilities/Logger.h"
#include "Harmony/Core/Registry.h"

namespace Harmony
{
    Harmony::Kernal::Kernal() = default;
    Harmony::Kernal::~Kernal() = default;

    void Kernal::initialize(const Properties& properties)
    {
        flecs::entity modules = world.entity("Kernel::Modules");
        properties.foreach({"modules"}, [&](const std::string& module_name, const Properties& module_properties)
        {
            if (std::unique_ptr<Controller> instance = Registry::create<Controller>(module_name)) {
                flecs::entity module = world.entity(instance->getType().c_str());
                module.add(flecs::Module);
                modules.add(module);

                module.set<std::unique_ptr<Controller>>(std::move(instance));
                controller(instance->getType()).initialize(module_properties);
            }

            else Logger::instance().critical("Harmony::Kernel::initialize - Failed to create module instance for '{}'", module_name);
        });
    }

    void Kernal::finalize()
    {
    world.each<std::unique_ptr<Controller>>([](std::unique_ptr<Controller>&  instance) {
            if (instance) return instance->finalize();
            Logger::instance().critical("Harmony::Kernel::finalize - Failed to finalize module instance");
        });
    }

    void Kernal::update()
    {
        world.each<std::unique_ptr<Controller>>([](std::unique_ptr<Controller>& instance) {
            if (instance) return instance->update();
            Logger::instance().critical("Harmony::Kernel::finalize - Failed to finalize module instance");
        });
    }

    void Kernal::render()
    {
        world.each<std::unique_ptr<Controller>>([](std::unique_ptr<Controller>& instance) {
            if (instance) return instance->render();
            Logger::instance().critical("Harmony::Kernel::finalize - Failed to finalize module instance");
        });
    }

    void Kernal::event()
    {
        world.each<std::unique_ptr<Controller>>([](std::unique_ptr<Controller>& instance) {
            if (instance) return instance->event();
            Logger::instance().critical("Harmony::Kernel::finalize - Failed to finalize module instance");
        });
    }

    Controller& Kernal::controller(const std::string& type) {
        return *world.entity(type.c_str()).get<std::unique_ptr<Controller>>();
    }

}
