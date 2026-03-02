cmake_minimum_required(VERSION 3.20)

project(Harmony LANGUAGES CXX)

# Explicitly listed source files
set(ENGINE_SOURCES
    source/Core/Extension.cpp
    source/Core/ContextLogger.cpp
    source/Core/Logger.cpp
    source/Extension/Engine.cpp
    source/Extension/Kernel.cpp
    source/Extension/RaylibWindow.cpp
    source/Utilities/Properties.cpp
)

set(ENGINE_HEADERS
    include/Harmony/Harmony.h
    include/Harmony/Core/Extension.h
    include/Harmony/Core/ContextLogger.h
    include/Harmony/Core/Logger.h
    include/Harmony/Core/Registry.h
    include/Harmony/Core/Registry.inl
    include/Harmony/Interfaces/IKernel.h
    include/Harmony/Interfaces/IKernel.inl
    include/Harmony/Interfaces/IEngine.h
    include/Harmony/Interfaces/IWindow.h
    include/Harmony/Utilities/Guarded.h
    include/Harmony/Utilities/Guarded.inl
    include/Harmony/Utilities/Logger.inl
    include/Harmony/Utilities/Properties.h
    include/Harmony/Utilities/Properties.inl
    source/Extension/Engine.h
    source/Extension/Kernel.h
    source/Extension/RaylibWindow.h
)

# Define the static library
add_library(Harmony OBJECT ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# Professional Scoping
target_link_libraries(Harmony
        PRIVATE
        flecs::flecs       # Hidden ECS implementation
        raylib             # Hidden implementation
        spdlog::spdlog     # Hidden logging
        glaze::glaze       # Hidden serialization
        PUBLIC
        glm::glm           # Exposed math for the Sandbox API
)

# Header management
target_include_directories(Harmony
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        "source"           # Internal headers
)

# Namespace alias for professional usage
add_library(Harmony::Engine ALIAS Harmony)