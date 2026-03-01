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
        properties.foreach({"modules"}, [&](const std::string& module_type, const Properties& module_properties)
        {
            const std::string module_name = module_properties.getKeys()
                .or_else([] { Logger::globalInstance().critical("Harmony::Kernel::initialize - Failed to get module name"); return std::optional<Properties::Keys>{}; })
                .value()[0];

            flecs::entity module = world.entity(module_type.c_str());
            module.add(flecs::Module);
            module.child_of(modules);

            if (std::unique_ptr<Controller> instance = Registry::create<Controller>(module_name)) {
                module.set<std::unique_ptr<Controller>>(std::move(instance));
                module.get<std::unique_ptr<Controller>>()->Initialize(properties);
            }

            else Logger::globalInstance().critical("Harmony::Kernel::initialize - Failed to create module instance for '{}'", module_name);

        });
    };

}
