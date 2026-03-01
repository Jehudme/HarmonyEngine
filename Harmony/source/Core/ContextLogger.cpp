#include "../../include/Harmony/Core/ContextLogger.h"

namespace Harmony
{
    Logger::Context::Context() = default;
    Logger::Context::~Context() = default;

    Logger* Logger::Context::get()
    {
        if (loggers.empty()) return nullptr;
        return loggers.top().first;
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

        if (loggers.empty()) {
            loggers.emplace(logger, 1); // FIX: Emplace is cleaner
            return;
        }

        if (loggers.top().first == logger) {
            loggers.top().second++;
        } else {
            loggers.emplace(logger, 1);
        }
    }

    void Logger::Context::pop(Logger* logger)
    {
        if (!logger) {
            Logger::global().error("Attempted to pop a null logger from the context stack. Operation ignored.");
            return;
        }

        if (loggers.empty()) {
            Logger::global().error("Attempted to pop from an empty context stack.");
            return;
        }

        // FIX: Ensure we are popping the correct logger
        if (loggers.top().first != logger) {
            Logger::global().error("Context stack corruption: Attempted to pop a logger that is not at the top.");
            return;
        }

        loggers.top().second--;
        if (loggers.top().second == 0) {
            loggers.pop();
        }
    }

    Logger::Context::ScopedGuard::ScopedGuard(Context& ctx, Logger* lgr)
        : context(&ctx), logger(lgr)
    {
        if (context) {
            context->push(logger);
        }
    }

    Logger::Context::ScopedGuard::~ScopedGuard()
    {
        if (context && logger) {
            context->pop(logger);
        }
    }

} // Harmony