#include "Harmony/Core/Controller.h"

namespace Harmony {
    Controller::Controller(Engine& engine) : engine(engine) {}
    Controller::~Controller() = default;

    void Controller::Initialize(const Properties& properties) {
        std::lock_guard<std::mutex> lock(m_mutex);

        std::optional<Properties> loggerProperties = properties.getSubProperties({"logger"});
        if (loggerProperties.has_value()) {
            logger = std::make_unique<Logger>(*loggerProperties);
        }

        logger->trace("Initializing... ");
        onInitialize(properties);

        logger->info("Initialized");
    }

    void Controller::Finalize() {
        std::lock_guard<std::mutex> lock(m_mutex);

        logger->info("Finalizing... ");
        onFinalize();

        logger->info("Finalized");
    }

    void Controller::update() {
        std::lock_guard<std::mutex> lock(m_mutex);

        logger->trace("Updating... ");
        onUpdate();

        logger->trace("Updated");
    }

    void Controller::render() {
        std::lock_guard<std::mutex> lock(m_mutex);

        logger->trace("Rendering... ");
        onRender();

        logger->trace("Rendered");
    }

    void Controller::event() {
        std::lock_guard<std::mutex> lock(m_mutex);

        logger->trace("Processing events... ");
        onEvent();

        logger->trace("Processed events");
    }
}