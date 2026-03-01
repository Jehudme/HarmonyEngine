#include "Harmony/Core/Extension.h"
#include "Harmony/Core/ContextLogger.h"

namespace Harmony {

    Extension::Extension(const std::string& name, const std::string& type, Engine& engine)
        : m_name(name), m_type(type), m_engine(engine),
          m_state(State::Shutdown),
          m_logger(std::make_unique<Logger>(name)) {}

    Extension::~Extension() = default;

    void Extension::initialize(const Properties& properties) {
        std::lock_guard<std::mutex> lock(m_mutex);

        // Allow the logger to be reconfigured from a "logger" sub-object in properties.
        if (auto loggerProperties = properties.getSubProperties({"logger"})) {
            m_logger = std::make_unique<Logger>(*loggerProperties);
        }

        // Push this extension's logger onto the thread-local context stack so that
        // any nested calls to Logger::context() resolve to this logger.
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;

        m_logger->trace("Extension '{}' of type '{}' is initializing...", m_name, m_type);
        onInitialize(properties);
        m_logger->info("Extension '{}' of type '{}' initialized successfully.", m_name, m_type);

        m_state.write([](State& s) { s = State::Initialized; });
    }

    void Extension::finalize() {
        std::lock_guard<std::mutex> lock(m_mutex);
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;

        m_logger->info("Extension '{}' of type '{}' is finalizing...", m_name, m_type);
        onFinalize();
        m_logger->info("Extension '{}' of type '{}' finalized.", m_name, m_type);
    }

    void Extension::update() {
        std::lock_guard<std::mutex> lock(m_mutex);
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;

        m_logger->trace("Extension '{}' of type '{}' is updating...", m_name, m_type);
        onUpdate();
        m_logger->trace("Extension '{}' of type '{}' update complete.", m_name, m_type);
    }

    void Extension::render() {
        std::lock_guard<std::mutex> lock(m_mutex);
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;

        m_logger->trace("Extension '{}' of type '{}' is rendering...", m_name, m_type);
        onRender();
        m_logger->trace("Extension '{}' of type '{}' render complete.", m_name, m_type);
    }

    void Extension::event() {
        std::lock_guard<std::mutex> lock(m_mutex);
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;

        m_logger->trace("Extension '{}' of type '{}' is processing events...", m_name, m_type);
        onEvent();
        m_logger->trace("Extension '{}' of type '{}' event processing complete.", m_name, m_type);
    }

    const std::string& Extension::getName() const { return m_name; }
    const std::string& Extension::getType() const { return m_type; }

} // namespace Harmony
