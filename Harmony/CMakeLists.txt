project(Harmony LANGUAGES CXX)

# Find source files
file(GLOB_RECURSE ENGINE_SOURCES "source/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS "include/*.h" "source/*.h")

# Define the static library
add_library(Harmony STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS}
        include/Harmony/Properties.h
        include/Harmony/Properties.inl)

# Professional Scoping
target_link_libraries(Harmony
        PRIVATE
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