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

namespace Harmony {

    struct Logger::Impl {
        std::shared_ptr<spdlog::logger> spd_logger;
    };


    Logger::Logger(const std::string& name, const Properties& properties) : pimpl(std::make_unique<Impl>()) {

        // 1. Extract Core Settings
        std::string lvl_str = properties.get<std::string>({"level"}).value_or(DEFAULT_LEVEL_STR);
        std::string pattern = properties.get<std::string>({"pattern"}).value_or(DEFAULT_PATTERN);
        std::string fls_str = properties.get<std::string>({"flush_on"}).value_or(DEFAULT_FLUSH_ON);

        // 2. Extract Async Settings
        bool async_enabled    = properties.get<bool>({"async", "enabled"}).value_or(DEFAULT_ASYNC_ENABLED);
        int32_t async_q_size  = properties.get<int32_t>({"async", "queue_size"}).value_or(DEFAULT_ASYNC_QUEUE_SIZE);
        int32_t async_threads = properties.get<int32_t>({"async", "thread_count"}).value_or(DEFAULT_ASYNC_THREAD_COUNT);

        // 3. Build Sinks
        std::vector<spdlog::sink_ptr> sinks;

        // Console Sink
        if (properties.get<bool>({"sinks", "console", "enabled"}).value_or(DEFAULT_SINK_CONSOLE_ENABLED)) {
            bool use_color = properties.get<bool>({"sinks", "console", "color"}).value_or(DEFAULT_SINK_CONSOLE_COLOR);
            if (use_color) {
                sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
            } else {
                // Assuming you want standard stdout if color is false (spdlog doesn't have a purely colorless stdout by default without overriding pattern, but this is a safe fallback)
                auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                console_sink->set_color_mode(spdlog::color_mode::never);
                sinks.push_back(console_sink);
            }
        }

        // Basic File Sink
        if (properties.get<bool>({"sinks", "basic_file", "enabled"}).value_or(DEFAULT_SINK_BASIC_ENABLED)) {
            std::string path = properties.get<std::string>({"Lsinks", "basic_file", "path"}).value_or(DEFAULT_SINK_BASIC_PATH);
            bool truncate    = properties.get<bool>({"sinks", "basic_file", "truncate"}).value_or(DEFAULT_SINK_BASIC_TRUNCATE);
            sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(path, truncate));
        }

        // Rotating File Sink
        if (properties.get<bool>({"sinks", "rotating_file", "enabled"}).value_or(DEFAULT_SINK_ROTATING_ENABLED)) {
            std::string path = properties.get<std::string>({"sinks", "rotating_file", "path"}).value_or(DEFAULT_SINK_ROTATING_PATH);
            int32_t max_mb   = properties.get<int32_t>({"sinks", "rotating_file", "max_size"}).value_or(DEFAULT_SINK_ROTATING_MAX_MB);
            int32_t max_files= properties.get<int32_t>({"sinks", "rotating_file", "max_files"}).value_or(DEFAULT_SINK_ROTATING_MAX_FILES);

            size_t max_bytes = static_cast<size_t>(max_mb) * 1024 * 1024;
            sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(path, max_bytes, max_files));
        }

        // Daily File Sink
        if (properties.get<bool>({"sinks", "daily_file", "enabled"}).value_or(DEFAULT_SINK_DAILY_ENABLED)) {
            std::string path = properties.get<std::string>({"sinks", "daily_file", "path"}).value_or(DEFAULT_SINK_DAILY_PATH);
            int32_t hour     = properties.get<int32_t>({"sinks", "daily_file", "hour"}).value_or(DEFAULT_SINK_DAILY_HOUR);
            int32_t minute   = properties.get<int32_t>({"sinks", "daily_file", "minute"}).value_or(DEFAULT_SINK_DAILY_MINUTE);
            sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>(path, hour, minute));
        }

        // 4. Construct the Logger (Async vs Sync)
        if (async_enabled) {
            spdlog::init_thread_pool(async_q_size, async_threads);
            pimpl->spd_logger = std::make_shared<spdlog::async_logger>(
                name, sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block
            );
        } else {
            pimpl->spd_logger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
        }

        // 5. Apply Core Settings
        pimpl->spd_logger->set_pattern(pattern);
        pimpl->spd_logger->set_level(spdlog::level::from_str(lvl_str));
        pimpl->spd_logger->flush_on(spdlog::level::from_str(fls_str));

        // 6. Register globally in spdlog registry
        if (name != DEFAULT_LOGGER_NAME) spdlog::register_logger(pimpl->spd_logger);
    }

    Logger::Logger(const Properties& properties) : Logger(DEFAULT_LOGGER_NAME, properties) {}

    Logger::~Logger() = default;

    Logger& Logger::global()
    {
        static Logger logger("Global");
        return logger;
    }

    Logger::Context& Logger::contextInstance()
    {
        // Each thread owns its own context stack so that concurrent extensions
        // do not interfere with each other's logger scope.
        static thread_local Context ctx;
        return ctx;
    }

    Logger& Logger::context()
    {
        Logger* logger = contextInstance().get();
        if (!logger) {
            // No extension has pushed a logger yet; warn once and fall back to the
            // global logger so callers always receive a valid reference.
            Logger::global().warn("No context logger found, falling back to global.");
            return Logger::global();
        }
        return *logger;
    }


    void Logger::dispatch_log(const Level level, const std::string& message) const
    {
        if (!pimpl || !pimpl->spd_logger) return;

        spdlog::level::level_enum spd_level = spdlog::level::info;

        switch (level) {
            case Level::TRACE:    spd_level = spdlog::level::trace; break;
            case Level::DEBUG:    spd_level = spdlog::level::debug; break;
            case Level::INFO:     spd_level = spdlog::level::info; break;
            case Level::WARNING:  spd_level = spdlog::level::warn; break;
            case Level::ERROR:    spd_level = spdlog::level::err; break;
            case Level::CRITICAL: spd_level = spdlog::level::critical; break;
        }

        pimpl->spd_logger->log(spd_level, message);
    }

}
