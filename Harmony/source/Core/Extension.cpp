#include "Harmony/Core/Extension.h"
#include "Harmony/Core/ContextLogger.h"
#include <array>

namespace Harmony {

    Extension::Extension(const std::string& name, const std::string& type, IKernel& kernel) :
        m_logger(std::make_unique<Logger>(name)),
        m_name(name), m_type(type), m_kernel(kernel),
        m_state(State::Shutdown) {}

    Extension::~Extension() = default;

    void Extension::initialize(const Properties& properties) {
        std::lock_guard<std::mutex> lock(m_lifecycleMutex);
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        onInitialize(properties);

        m_state.store(State::Initialized, std::memory_order_release);
    }

    void Extension::finalize() {
        std::lock_guard<std::mutex> lock(m_lifecycleMutex);
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        onFinalize();

        m_state.store(State::Shutdown, std::memory_order_release);
    }

    void Extension::update() {
        std::lock_guard<std::mutex> lock(m_lifecycleMutex);
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        onUpdate();
    }

    void Extension::render() {
        std::lock_guard<std::mutex> lock(m_lifecycleMutex);
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        onRender();
    }

    void Extension::event() {
        std::lock_guard<std::mutex> lock(m_lifecycleMutex);
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        onEvent();
    }

    const std::string& Extension::getName() const { return m_name; }
    const std::string& Extension::getType() const { return m_type; }

} // namespace Harmony
