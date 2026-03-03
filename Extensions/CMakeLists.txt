project(HarmonyExtensions LANGUAGES CXX)

# Define the static library
add_library(Extensions OBJECT
        Runner/source/Runner.cpp
        Kernel/source/Kernel.cpp
        Window/source/Window.cpp
        Window/source/Window.h
)

# Links to the Core Engine (Interfaces) and implementation dependencies
target_link_libraries(Extensions
        PUBLIC
        Harmony::Engine    # Public so Sandbox inherits Core headers
        PRIVATE
        raylib             # Required for Window implementation
        flecs::flecs       # Required for Kernel automation
)

add_library(Harmony::Extensions ALIAS Extensions)