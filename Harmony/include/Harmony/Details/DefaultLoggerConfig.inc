#pragma once
#include <cstdint>

namespace Harmony
{
    constexpr const char* DEFAULT_LOGGER_NAME = "Unknow";
    constexpr const char* DEFAULT_LEVEL_STR = "info";
    constexpr const char* DEFAULT_PATTERN = "[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v";
    constexpr const char* DEFAULT_FLUSH_ON = "err";

    constexpr bool    DEFAULT_ASYNC_ENABLED = false;
    constexpr int32_t DEFAULT_ASYNC_QUEUE_SIZE = 8192;
    constexpr int32_t DEFAULT_ASYNC_THREAD_COUNT = 1;

    constexpr bool    DEFAULT_SINK_CONSOLE_ENABLED = true;
    constexpr bool    DEFAULT_SINK_CONSOLE_COLOR = true;

    constexpr bool    DEFAULT_SINK_BASIC_ENABLED = false;
    constexpr const char* DEFAULT_SINK_BASIC_PATH = "logs/basic.log";
    constexpr bool    DEFAULT_SINK_BASIC_TRUNCATE = false;

    constexpr bool    DEFAULT_SINK_ROTATING_ENABLED = false;
    constexpr const char* DEFAULT_SINK_ROTATING_PATH = "logs/rotating.log";
    constexpr int32_t DEFAULT_SINK_ROTATING_MAX_MB = 5;
    constexpr int32_t DEFAULT_SINK_ROTATING_MAX_FILES = 3;

    constexpr bool    DEFAULT_SINK_DAILY_ENABLED = false;
    constexpr const char* DEFAULT_SINK_DAILY_PATH = "logs/daily.log";
    constexpr int32_t DEFAULT_SINK_DAILY_HOUR = 0;
    constexpr int32_t DEFAULT_SINK_DAILY_MINUTE = 0;
}
