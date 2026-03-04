cmake_minimum_required(VERSION 3.20)

project(Harmony LANGUAGES CXX)

# Define the static library
add_library(Harmony OBJECT
        source/Core/Extension.cpp
        source/Core/ContextLogger.cpp
        source/Core/Logger.cpp
        source/Utilities/Properties.cpp
        include/Harmony/Harmony.h
        include/Harmony/Core/Extension.h
        include/Harmony/Core/ContextLogger.h
        include/Harmony/Core/Logger.h
        include/Harmony/Core/Registry.h
        include/Harmony/Core/Registry.inl
        include/Harmony/Interfaces/IKernel.h
        include/Harmony/Interfaces/IKernel.inl
        include/Harmony/Interfaces/IRunner.h
        include/Harmony/Interfaces/IWindow.h
        include/Harmony/Utilities/Guarded.h
        include/Harmony/Utilities/Guarded.inl
        include/Harmony/Utilities/Logger.inl
        include/Harmony/Utilities/Properties.h
        include/Harmony/Utilities/Properties.inl
        include/Harmony/Interfaces/IStateStack.h
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