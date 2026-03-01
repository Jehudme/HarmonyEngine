#pragma once
#include <Harmony/Core/Logger.h> // Assuming 'class Logger { class Context; };' is set up here
#include <utility>
#include <stack>

namespace Harmony
{
    class Logger::Context
    {
    public:
        class ScopedGuard;
        friend class Logger;
        friend class ScopedGuard;

        ~Context();

        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;

        Context(Context&&) noexcept = delete;
        Context& operator=(Context&&) noexcept = delete;

        ScopedGuard createGuard(Logger* logger);

    private:
        Context();

        Logger* get();

        void push(Logger* logger);
        void pop(Logger* logger);

        std::stack<std::pair<Logger*, uint16_t>> m_loggers;
    };

    class Logger::Context::ScopedGuard
    {
        friend class Context;

    public:
        ScopedGuard(const ScopedGuard&) = delete;
        ScopedGuard& operator=(const ScopedGuard&) = delete;

        ScopedGuard(ScopedGuard&&) noexcept = default;
        ScopedGuard& operator=(ScopedGuard&&) noexcept = default;

        ~ScopedGuard();

    private:
        ScopedGuard(Context& loggerContext, Logger* targetLogger);

        Context* m_context;
        Logger* m_logger;
    };
}

// Creates a named RAII guard that pushes this extension's logger onto the
// thread-local context stack for the lifetime of the enclosing scope.
// Using a named variable (not a bare temporary) ensures the pop happens at
// end-of-scope rather than end-of-statement.
#define HARMONY_EXTENSION_CONTEXT_LOGGER_GUARD \
    auto _harmony_ctx_guard = Harmony::Logger::contextInstance().createGuard(m_logger.get())
