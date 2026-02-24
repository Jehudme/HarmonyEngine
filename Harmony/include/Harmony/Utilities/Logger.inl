#pragma once

namespace Harmony {

    template<typename... Args>
    inline void Logger::trace(std::format_string<Args...> format, Args&&... args) {
        dispatch_log(Level::TRACE, std::format(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    inline void Logger::debug(std::format_string<Args...> format, Args&&... args) {
        dispatch_log(Level::DEBUG, std::format(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    inline void Logger::info(std::format_string<Args...> format, Args&&... args) {
        dispatch_log(Level::INFO, std::format(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    inline void Logger::warn(std::format_string<Args...> format, Args&&... args) {
        dispatch_log(Level::WARNING, std::format(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    inline void Logger::error(std::format_string<Args...> format, Args&&... args) {
        dispatch_log(Level::ERROR, std::format(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    inline void Logger::critical(std::format_string<Args...> format, Args&&... args) {
        dispatch_log(Level::CRITICAL, std::format(format, std::forward<Args>(args)...));
    }
}