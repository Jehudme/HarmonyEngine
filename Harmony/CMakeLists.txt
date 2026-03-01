project(Harmony LANGUAGES CXX)

# Find source files
file(GLOB_RECURSE ENGINE_SOURCES "source/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS "include/*.h" "source/*.h")

# Define the static library
add_library(Harmony STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS}
        include/Harmony/Utilities/Properties.h
        include/Harmony/Utilities/Properties.inl
        source/Utilities/Logger.cpp
        include/Harmony/Utilities/Logger.h
        include/Harmony/Utilities/Logger.inl
        include/Harmony/Utilities/Guarded.h
        include/Harmony/Core/Controller.h
        include/Harmony/Core/Service.h
        source/Core/Controller.cpp
        source/Core/Service.cpp
        include/Harmony/Interfaces/IWindow.h
        include/Harmony/Utilities/Uncopyable.h
        include/Harmony/Core/Registry.h
        include/Harmony/Core/Registry.inl
        source/Core/Engine.cpp
        include/Harmony/Core/Engine.h
        include/Harmony/Interfaces/IKernel.h
        source/Modules/Kernal.cpp
        source/Modules/Kernal.h
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