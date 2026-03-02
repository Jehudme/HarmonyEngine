cmake_minimum_required(VERSION 3.20)

project(Harmony LANGUAGES CXX)

# Find source files
file(GLOB_RECURSE ENGINE_SOURCES "source/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS "include/*.h" "source/*.h")

# Define the static library
add_library(Harmony OBJECT ${ENGINE_SOURCES} ${ENGINE_HEADERS}
        include/Harmony/Utilities/Properties.h
        include/Harmony/Utilities/Properties.inl
        source/Core/Logger.cpp
        include/Harmony/Core/Logger.h
        include/Harmony/Utilities/Logger.inl
        include/Harmony/Utilities/Guarded.h
        include/Harmony/Core/Extension.h
        source/Core/Extension.cpp
        include/Harmony/Interfaces/IWindow.h
        include/Harmony/Core/Registry.h
        include/Harmony/Core/Registry.inl
        source/Extension/Engine.cpp
        source/Extension/Engine.h
        include/Harmony/Interfaces/IKernel.h
        source/Extension/Kernel.h
        source/Extension/Kernel.cpp
        source/Extension/RaylibWindow.cpp
        source/Extension/RaylibWindow.h
        source/Core/ContextLogger.cpp
        include/Harmony/Core/ContextLogger.h
        include/Harmony/Interfaces/IEngine.h
        include/Harmony/Interfaces/IKernel.inl
)

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