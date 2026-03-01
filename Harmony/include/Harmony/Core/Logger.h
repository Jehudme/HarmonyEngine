#pragma once
#include <string>
#include <memory>
#include <format>
#include <string_view>

#include <Harmony/Utilities/Properties.h>

namespace Harmony {

    // Logger provides a flexible, configurable logging system backed by spdlog.
    // Supports multiple sinks (console, file, rotating file, daily file) and async mode.
    // Thread-local context stacks enable per-extension logging scopes.
    class Logger {
    public:
        enum class Level { TRACE, DEBUG, INFO, WARNING, ERROR, CRITICAL };

        explicit Logger(const std::string& name, const Properties& properties = Properties());
        explicit Logger(const Properties& properties = Properties());

        ~Logger();

        template<typename... Args>
        void trace(std::format_string<Args...> format, Args&&... args);

        template<typename... Args>
        void debug(std::format_string<Args...> format, Args&&... args);

        template<typename... Args>
        void info(std::format_string<Args...> format, Args&&... args);

        template<typename... Args>
        void warn(std::format_string<Args...> format, Args&&... args);

        template<typename... Args>
        void error(std::format_string<Args...> format, Args&&... args);

        template<typename... Args>
        void critical(std::format_string<Args...> format, Args&&... args);

        // Returns the global singleton logger instance.
        static Logger& global();

        // Returns the current thread's context logger, falling back to global if none is set.
        static Logger& context();

        class Context;

        // Returns the thread-local Context instance used for push/pop guard operations.
        static Context& contextInstance();

    private:
        void dispatch_log(Level level, const std::string& message) const;

        struct Impl;
        std::unique_ptr<Impl> pimpl;
    };

}

#include <Harmony/Utilities/Logger.inl>
