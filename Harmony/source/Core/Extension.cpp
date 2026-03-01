#include "Harmony/Core/Extension.h"
#include "Harmony/Core/ContextLogger.h"
#include <array>

namespace Harmony {

    Extension::Extension(const std::string& name, const std::string& type, Engine& engine)
        : m_name(name), m_type(type), m_engine(engine),
          m_state(State::Shutdown),
          m_logger(std::make_unique<Logger>(name)) {}

    Extension::~Extension() = default;

    void Extension::initialize(const Properties& properties) {
        std::lock_guard<std::mutex> lock(m_lifecycleMutex);

        // Allow the logger to be reconfigured from a "logger" sub-object in properties.
        // This enables per-extension logging configuration via JSON.
        constexpr std::array<std::string_view, 1> loggerPath = {"logger"};
        if (auto loggerProperties = properties.getSubProperties(std::span<const std::string_view>(loggerPath))) {
            m_logger = std::make_unique<Logger>(*loggerProperties);
        }

        // Push this extension's logger onto the thread-local context stack so that
        // any nested calls to Logger::context() resolve to this extension's logger.
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;

        m_logger->trace("Extension '{}' of type '{}' beginning initialization...", m_name, m_type);
        onInitialize(properties);
        m_logger->info("Extension '{}' of type '{}' initialized successfully.", m_name, m_type);

        m_state.store(State::Initialized, std::memory_order_release);
    }

    void Extension::finalize() {
        std::lock_guard<std::mutex> lock(m_lifecycleMutex);
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;

        m_logger->info("Extension '{}' of type '{}' beginning finalization...", m_name, m_type);
        onFinalize();
        m_logger->info("Extension '{}' of type '{}' finalization complete.", m_name, m_type);

        m_state.store(State::Shutdown, std::memory_order_release);
    }

    void Extension::update(float deltaTime) {
        std::lock_guard<std::mutex> lock(m_lifecycleMutex);
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;

        m_logger->trace("Extension '{}' of type '{}' processing update frame...", m_name, m_type);
        onUpdate(deltaTime);
    }

    void Extension::render() {
        std::lock_guard<std::mutex> lock(m_lifecycleMutex);
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;

        m_logger->trace("Extension '{}' of type '{}' processing render frame...", m_name, m_type);
        onRender();
    }

    void Extension::event() {
        std::lock_guard<std::mutex> lock(m_lifecycleMutex);
        HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD;

        m_logger->trace("Extension '{}' of type '{}' processing events...", m_name, m_type);
        onEvent();
    }

    const std::string& Extension::getName() const { return m_name; }
    const std::string& Extension::getType() const { return m_type; }

} // namespace Harmony
