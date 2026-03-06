#include <iostream>
#include <filesystem>
#include <array>
#include <string_view>
#include <Harmony/Harmony.h>

int main()
{
    Harmony::Logger mainLogger("Main");
    HARMONY_CONTEXT_LOGGER_GUARD(&mainLogger);

    auto properties = Harmony::Properties("assets/application.json");
    auto kernel = Harmony::Registry::create<Harmony::IKernel>("kernel");

    kernel->initialize(properties);
    kernel->getExtension<Harmony::IRunner>()->start();

    sleep(10);

    kernel->getExtension<Harmony::IRunner>()->stop();

}