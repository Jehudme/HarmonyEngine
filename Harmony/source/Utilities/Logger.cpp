#include "../../include/Harmony/Core/Logger.h"
#include "Harmony/Details/DefaultLoggerConfig.inc"

// spdlog includes
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>

#include "Harmony/Core/ContextLogger.h"
#include <array>
#include <span>

namespace Harmony {

    struct Logger::Impl {
        std::shared_ptr<spdlog::logger> spd_logger;
    };

    Logger::Logger(const std::string& name, const Properties& properties) : pimpl(std::make_unique<Impl>()) {

        // 1. Extract Core Settings
        constexpr std::array<std::string_view, 1> levelPath = {"level"};
        constexpr std::array<std::string_view, 1> patternPath = {"pattern"};
        constexpr std::array<std::string_view, 1> flushOnPath = {"flush_on"};
        std::string levelString = properties.get<std::string>(std::span<const std::string_view>(levelPath)).value_or(DEFAULT_LEVEL_STR);
        std::string logPattern = properties.get<std::string>(std::span<const std::string_view>(patternPath)).value_or(DEFAULT_PATTERN);
        std::string flushOnLevel = properties.get<std::string>(std::span<const std::string_view>(flushOnPath)).value_or(DEFAULT_FLUSH_ON);

        // 2. Extract Async Settings
        constexpr std::array<std::string_view, 2> asyncEnabledPath = {"async", "enabled"};
        constexpr std::array<std::string_view, 2> asyncQueueSizePath = {"async", "queue_size"};
        constexpr std::array<std::string_view, 2> asyncThreadCountPath = {"async", "thread_count"};
        bool asyncEnabled = properties.get<bool>(std::span<const std::string_view>(asyncEnabledPath)).value_or(DEFAULT_ASYNC_ENABLED);
        int32_t asyncQueueSize = properties.get<int32_t>(std::span<const std::string_view>(asyncQueueSizePath)).value_or(DEFAULT_ASYNC_QUEUE_SIZE);
        int32_t asyncThreadCount = properties.get<int32_t>(std::span<const std::string_view>(asyncThreadCountPath)).value_or(DEFAULT_ASYNC_THREAD_COUNT);

        // 3. Build Sinks
        std::vector<spdlog::sink_ptr> sinks;

        // Console Sink
        constexpr std::array<std::string_view, 3> consoleEnabledPath = {"sinks", "console", "enabled"};
        constexpr std::array<std::string_view, 3> consoleColorPath = {"sinks", "console", "color"};
        if (properties.get<bool>(std::span<const std::string_view>(consoleEnabledPath)).value_or(DEFAULT_SINK_CONSOLE_ENABLED)) {
            bool useColor = properties.get<bool>(std::span<const std::string_view>(consoleColorPath)).value_or(DEFAULT_SINK_CONSOLE_COLOR);
            auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            if (!useColor) {
                consoleSink->set_color_mode(spdlog::color_mode::never);
            }
            sinks.push_back(consoleSink);
        }

        // Basic File Sink
        constexpr std::array<std::string_view, 3> basicEnabledPath = {"sinks", "basic_file", "enabled"};
        constexpr std::array<std::string_view, 3> basicPathPath = {"sinks", "basic_file", "path"};
        constexpr std::array<std::string_view, 3> basicTruncatePath = {"sinks", "basic_file", "truncate"};
        if (properties.get<bool>(std::span<const std::string_view>(basicEnabledPath)).value_or(DEFAULT_SINK_BASIC_ENABLED)) {
            std::string filePath = properties.get<std::string>(std::span<const std::string_view>(basicPathPath)).value_or(DEFAULT_SINK_BASIC_PATH);
            bool truncateOnOpen = properties.get<bool>(std::span<const std::string_view>(basicTruncatePath)).value_or(DEFAULT_SINK_BASIC_TRUNCATE);
            sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(filePath, truncateOnOpen));
        }

        // Rotating File Sink
        constexpr std::array<std::string_view, 3> rotatingEnabledPath = {"sinks", "rotating_file", "enabled"};
        constexpr std::array<std::string_view, 3> rotatingPathPath = {"sinks", "rotating_file", "path"};
        constexpr std::array<std::string_view, 3> rotatingMaxSizePath = {"sinks", "rotating_file", "max_size"};
        constexpr std::array<std::string_view, 3> rotatingMaxFilesPath = {"sinks", "rotating_file", "max_files"};
        if (properties.get<bool>(std::span<const std::string_view>(rotatingEnabledPath)).value_or(DEFAULT_SINK_ROTATING_ENABLED)) {
            std::string filePath = properties.get<std::string>(std::span<const std::string_view>(rotatingPathPath)).value_or(DEFAULT_SINK_ROTATING_PATH);
            int32_t maxSizeMb = properties.get<int32_t>(std::span<const std::string_view>(rotatingMaxSizePath)).value_or(DEFAULT_SINK_ROTATING_MAX_MB);
            int32_t maxFileCount = properties.get<int32_t>(std::span<const std::string_view>(rotatingMaxFilesPath)).value_or(DEFAULT_SINK_ROTATING_MAX_FILES);

            size_t maxSizeBytes = static_cast<size_t>(maxSizeMb) * 1024 * 1024;
            sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filePath, maxSizeBytes, maxFileCount));
        }

        // Daily File Sink
        constexpr std::array<std::string_view, 3> dailyEnabledPath = {"sinks", "daily_file", "enabled"};
        constexpr std::array<std::string_view, 3> dailyPathPath = {"sinks", "daily_file", "path"};
        constexpr std::array<std::string_view, 3> dailyHourPath = {"sinks", "daily_file", "hour"};
        constexpr std::array<std::string_view, 3> dailyMinutePath = {"sinks", "daily_file", "minute"};
        if (properties.get<bool>(std::span<const std::string_view>(dailyEnabledPath)).value_or(DEFAULT_SINK_DAILY_ENABLED)) {
            std::string filePath = properties.get<std::string>(std::span<const std::string_view>(dailyPathPath)).value_or(DEFAULT_SINK_DAILY_PATH);
            int32_t rotationHour = properties.get<int32_t>(std::span<const std::string_view>(dailyHourPath)).value_or(DEFAULT_SINK_DAILY_HOUR);
            int32_t rotationMinute = properties.get<int32_t>(std::span<const std::string_view>(dailyMinutePath)).value_or(DEFAULT_SINK_DAILY_MINUTE);
            sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>(filePath, rotationHour, rotationMinute));
        }

        // 4. Construct the Logger (Async vs Sync)
        if (asyncEnabled) {
            spdlog::init_thread_pool(asyncQueueSize, asyncThreadCount);
            pimpl->spd_logger = std::make_shared<spdlog::async_logger>(
                name, sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block
            );
        } else {
            pimpl->spd_logger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
        }

        // 5. Apply Core Settings
        pimpl->spd_logger->set_pattern(logPattern);
        pimpl->spd_logger->set_level(spdlog::level::from_str(levelString));
        pimpl->spd_logger->flush_on(spdlog::level::from_str(flushOnLevel));

        // 6. Register globally in spdlog registry (except for default logger)
        if (name != DEFAULT_LOGGER_NAME) {
            spdlog::register_logger(pimpl->spd_logger);
        }
    }

    Logger::Logger(const Properties& properties) : Logger(DEFAULT_LOGGER_NAME, properties) {}

    Logger::~Logger() = default;

    Logger& Logger::global()
    {
        static Logger globalLogger("Global");
        return globalLogger;
    }

    Logger::Context& Logger::contextInstance()
    {
        // Each thread owns its own context stack so concurrent extensions
        // do not interfere with each other's logger scope.
        static thread_local Context threadLocalContext;
        return threadLocalContext;
    }

    Logger& Logger::context()
    {
        Logger* contextLogger = contextInstance().get();
        if (!contextLogger) {
            // No extension has pushed a logger yet; warn once and fall back to
            // the global logger so callers always receive a valid reference.
            Logger::global().warn("Logger::context - No context logger found on current thread; falling back to global logger.");
            return Logger::global();
        }
        return *contextLogger;
    }

    void Logger::dispatch_log(const Level level, const std::string& message) const
    {
        if (!pimpl || !pimpl->spd_logger) {
            return;
        }

        spdlog::level::level_enum spdLevel = spdlog::level::info;

        switch (level) {
            case Level::TRACE:    spdLevel = spdlog::level::trace; break;
            case Level::DEBUG:    spdLevel = spdlog::level::debug; break;
            case Level::INFO:     spdLevel = spdlog::level::info; break;
            case Level::WARNING:  spdLevel = spdlog::level::warn; break;
            case Level::ERROR:    spdLevel = spdlog::level::err; break;
            case Level::CRITICAL: spdLevel = spdlog::level::critical; break;
        }

        pimpl->spd_logger->log(spdLevel, message);
    }

}
