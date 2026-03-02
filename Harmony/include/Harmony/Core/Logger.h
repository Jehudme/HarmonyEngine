#pragma once
#include <string>
#include <memory>
#include <format>
#include <string_view>

#include <Harmony/Utilities/Properties.h>

namespace Harmony {
    class Logger {
    public:
        enum class Level { TRACE, DEBUG, INFO, WARNING, ERROR, CRITICAL };

        explicit Logger(const std::string& name, const Properties& properties);
        explicit Logger(const Properties& properties = Properties());
        Logger(const std::string& name);
        ~Logger();

        Logger& operator=(const Logger&) = delete;
        Logger(const Logger&) = delete;

        Logger& operator=(Logger&&) noexcept = delete;
        Logger(Logger&&) noexcept = delete;

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

        static Logger& global();
        static Logger& context();

        class Context;
        static Context& contextInstance();

        // Compatibility helpers: allow value-type Logger to be used
        // where code previously relied on unique_ptr<Logger> interface.
        Logger* get() { return this; }
        const Logger* get() const { return this; }
        Logger* operator->() { return this; }
        const Logger* operator->() const { return this; }

    private:
        void dispatch_log(Level level, const std::string& message) const;

        struct Impl;
        std::unique_ptr<Impl> pimpl;
    };

}

#include <Harmony/Utilities/Logger.inl>
