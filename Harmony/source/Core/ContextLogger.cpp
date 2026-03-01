#include "../../include/Harmony/Core/ContextLogger.h"

namespace Harmony
{
    Logger::Context::Context() = default;
    Logger::Context::~Context() = default;

    Logger* Logger::Context::get()
    {
        if (m_loggers.empty()) return nullptr;
        return m_loggers.top().first;
    }

    Logger::Context::ScopedGuard Logger::Context::createGuard(Logger* logger)
    {
        return ScopedGuard(*this, logger);
    }

    void Logger::Context::push(Logger* logger)
    {
        if (!logger) {
            Logger::global().error("Attempted to push a null logger onto the context stack. Operation ignored.");
            return; // FIX: Added return
        }

        if (m_loggers.empty()) {
            m_loggers.emplace(logger, 1); // FIX: Emplace is cleaner
            return;
        }

        if (m_loggers.top().first == logger) {
            m_loggers.top().second++;
        } else {
            m_loggers.emplace(logger, 1);
        }
    }

    void Logger::Context::pop(Logger* logger)
    {
        if (!logger) {
            Logger::global().error("Attempted to pop a null logger from the context stack. Operation ignored.");
            return;
        }

        if (m_loggers.empty()) {
            Logger::global().error("Attempted to pop from an empty context stack.");
            return;
        }

        // FIX: Ensure we are popping the correct logger
        if (m_loggers.top().first != logger) {
            Logger::global().error("Context stack corruption: Attempted to pop a logger that is not at the top.");
            return;
        }

        m_loggers.top().second--;
        if (m_loggers.top().second == 0) {
            m_loggers.pop();
        }
    }

    Logger::Context::ScopedGuard::ScopedGuard(Context& loggerContext, Logger* targetLogger)
        : m_context(&loggerContext), m_logger(targetLogger)
    {
        if (m_context) {
            m_context->push(m_logger);
        }
    }

    Logger::Context::ScopedGuard::~ScopedGuard()
    {
        if (m_context && m_logger) {
            m_context->pop(m_logger);
        }
    }

} // Harmony