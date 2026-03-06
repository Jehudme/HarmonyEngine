#include "flecs.h"
#include "Kernel.h"

#include "Harmony/Core/Extension.h"
#include "Harmony/Core/Registry.h"
#include "Harmony/Interfaces/IScene.h"

namespace Harmony
{
    Kernel::Kernel() : IKernel(GET_NAME(), *this), m_world() { setupAutomation(); }
    Kernel::~Kernel() = default;

    void Kernel::initialize(const Properties& properties)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        flecs::entity extensionRoot = m_world.entity("Kernel::Extensions");

        properties.foreach({"extensions"}, [&](const std::string& extensionName, const Properties& extensionProperties)
        {
            std::unique_ptr<Extension> extensionInstance = Registry::create<Extension, IKernel&>(extensionName, *this);

            if (extensionInstance)
            {
                const std::string& extensionType = extensionInstance->getType();

                flecs::entity extensionEntity = m_world.entity(extensionType.c_str());
                extensionEntity.set<std::unique_ptr<Extension>>(std::move(extensionInstance));

                extensionEntity.add(flecs::Module);
                extensionRoot.add(extensionEntity);

                getExtension(extensionType)->initialize(extensionProperties);
                m_logger->info("Initialized extension '{}'", extensionName);

            } else {
                m_logger->error("Failed to initialize extension '{}'", extensionName);
            }
        });
    }

    void Kernel::createScene(const std::string& name)
    {
        flecs::entity scenes = m_world.entity("Kernel::Scenes"); m_world.scope(scenes);
        Properties properties = getProperties().getSubProperties({"scenes", name}).value();

        flecs::entity scene = m_world.entity(name.c_str());
        std::unique_ptr<IScene> sceneInstance = Registry::create<IScene, IKernel&>(name, *this);
    }

    void Kernel::deleteScene(const std::string& name)
    {
        flecs::entity scenes = m_world.entity("Kernel::Scenes");

    }

    bool Kernel::getSceneActive(const std::string& name)
    {
        flecs::entity scenes = m_world.entity("Kernel::Scenes");

    }

    void Kernel::setSceneActive(const std::string& name, bool option)
    {
    }

    void Kernel::setupAutomation() {
        // EVENT System: Runs at the start of the frame (OnLoad)
        m_world.system<std::unique_ptr<Extension>>("Extension::Events")
            .kind(flecs::OnLoad)
            .each([](std::unique_ptr<Extension>& ext) {
                ext->event();
            });

        // UPDATE System: Runs in the middle (OnUpdate)
        m_world.system<std::unique_ptr<Extension>>("Extension::Update")
            .kind(flecs::OnUpdate)
            .each([](std::unique_ptr<Extension>& ext) {
                ext->update();
            });

        // RENDER System: Runs at the end (OnStore)
        m_world.system<std::unique_ptr<Extension>>("Extension::Render")
            .kind(flecs::OnStore)
            .each([](std::unique_ptr<Extension>& ext) {
                ext->render();
            });
    }

    void Kernel::update()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_world.progress();
    }

    Extension* Kernel::getExtension(const std::string& extensionType) {
        return m_world.entity(extensionType.c_str()).get<std::unique_ptr<Extension>>().get();
    }

    HARMONY_REGISTER(IKernel, Kernel, "kernel");

}
