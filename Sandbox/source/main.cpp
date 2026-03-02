#include <iostream>
#include <filesystem>
#include <array>
#include <string_view>
#include <Harmony/Harmony.h>

int main()
{
    Harmony::Logger mainLogger("Main");
    HARMONY_CONTEXT_LOGGER_GUARD(&mainLogger);

    Harmony::Properties properties;
    properties.load("assets/application.json");

    // Pass keys using the new initializer_list-friendly Path syntax
    if (!properties.getKeys({"extensions"}).has_value()) {
        mainLogger.error("CRITICAL ERROR: Failed to load application.json!");
        mainLogger.error("Current Working Directory is: {}", std::filesystem::current_path().string());
        return -1;
    }

    auto kernel = Harmony::Registry::create<Harmony::IKernel>("kernel");

    // Remember the hidden bug fix here: pass *kernel instead of properties!
    kernel->initialize(properties);

    while (true) {
        kernel->progress();
    }
}