#include "../../include/Harmony/Core/ContextLogger.h"

namespace Harmony
{
    Logger::Context::Context() = default;
    Logger::Context::~Context() = default;

    Logger* Logger::Context::get()
    {
        if (m_loggers.empty()) {
            return nullptr;
        }
        return m_loggers.top().first;
    }

    Logger::Context::ScopedGuard Logger::Context::createGuard(Logger* logger)
    {
        return ScopedGuard(*this, logger);
    }

    void Logger::Context::push(Logger* logger)
    {
        if (!logger) {
            Logger::global().error("Logger::Context::push - Attempted to push null logger onto context stack; operation ignored.");
            return;
        }

        // Reuse the top entry if the same logger is being pushed consecutively.
        // This avoids stack bloat when the same extension makes nested calls.
        if (m_loggers.empty()) {
            m_loggers.emplace(logger, 1);
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
            Logger::global().error("Logger::Context::pop - Attempted to pop null logger from context stack; operation ignored.");
            return;
        }

        if (m_loggers.empty()) {
            Logger::global().error("Logger::Context::pop - Attempted to pop from empty context stack; possible mismatched push/pop calls.");
            return;
        }

        // Validate we are popping the correct logger to detect stack corruption.
        if (m_loggers.top().first != logger) {
            Logger::global().error("Logger::Context::pop - Stack corruption detected: attempted to pop logger that is not at stack top.");
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

} // namespace Harmony