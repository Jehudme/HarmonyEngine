#include "flecs.h"
#include "Harmony/Core/Kernel.h"

#include "Harmony/Core/Extension.h"
#include "Harmony/Core/Engine.h"
#include "Harmony/Core/Logger.h"
#include "Harmony/Core/Registry.h"

namespace Harmony
{
    Kernel::Kernel() : m_world(std::make_unique<flecs::world>()) {}
    Kernel::~Kernel() = default;

    void Kernel::initialize(Engine& engine, const Properties& properties)
    {
        Logger::global().info("Kernel initializing with module configuration from properties...");

        // Create a root entity to group all modules under a single ECS parent.
        flecs::entity modulesRoot = m_world->entity("Kernel::Modules");

        // Iterate over each module defined in the "modules" section of the config.
        properties.foreach({"modules"}, [&](const std::string& moduleName, const Properties& moduleProperties)
        {
            // Attempt to instantiate the extension via the type registry.
            std::unique_ptr<Extension> extensionInstance = Registry::create<Extension>(moduleName, engine);

            if (extensionInstance) {
                const std::string extensionType = extensionInstance->getType();
                Logger::global().info("Kernel: Creating module '{}' of type '{}'.", moduleName, extensionType);

                // Register the extension as a Flecs module entity.
                flecs::entity moduleEntity = m_world->entity(extensionType.c_str());
                moduleEntity.add(flecs::Module);
                modulesRoot.add(moduleEntity);

                // Transfer ownership of the extension to the ECS world.
                moduleEntity.set<std::unique_ptr<Extension>>(std::move(extensionInstance));

                // Initialize the extension with its specific properties.
                extension(extensionType).initialize(moduleProperties);
                Logger::global().info("Kernel: Module '{}' of type '{}' initialized.", moduleName, extensionType);
            }
            else {
                Logger::global().critical("Kernel::initialize - Failed to create extension instance for module '{}'; verify it is registered.", moduleName);
            }
        });

        Logger::global().info("Kernel initialization complete.");
    }

    void Kernel::finalize()
    {
        Logger::global().info("Kernel finalizing all registered extensions...");

        m_world->each<std::unique_ptr<Extension>>([](std::unique_ptr<Extension>& extensionInstance) {
            if (extensionInstance) {
                return extensionInstance->finalize();
            }
            Logger::global().critical("Kernel::finalize - Encountered null extension instance during finalization.");
        });

        Logger::global().info("Kernel finalization complete.");
    }

    void Kernel::update()
    {
        // Propagate update call to all registered extensions in ECS iteration order.
        m_world->each<std::unique_ptr<Extension>>([](std::unique_ptr<Extension>& extensionInstance) {
            if (extensionInstance) {
                return extensionInstance->update();
            }
            Logger::global().critical("Kernel::update - Encountered null extension instance during update.");
        });
    }

    void Kernel::render()
    {
        // Propagate render call to all registered extensions in ECS iteration order.
        m_world->each<std::unique_ptr<Extension>>([](std::unique_ptr<Extension>& extensionInstance) {
            if (extensionInstance) {
                return extensionInstance->render();
            }
            Logger::global().critical("Kernel::render - Encountered null extension instance during render.");
        });
    }

    void Kernel::event()
    {
        // Propagate event processing to all registered extensions in ECS iteration order.
        m_world->each<std::unique_ptr<Extension>>([](std::unique_ptr<Extension>& extensionInstance) {
            if (extensionInstance) {
                return extensionInstance->event();
            }
            Logger::global().critical("Kernel::event - Encountered null extension instance during event processing.");
        });
    }

    Extension& Kernel::extension(const std::string& extensionType) {
        // Retrieve the extension component from the ECS entity named by extensionType.
        return *m_world->entity(extensionType.c_str()).get<std::unique_ptr<Extension>>();
    }

}
