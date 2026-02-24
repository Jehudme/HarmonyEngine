project(Sandbox LANGUAGES CXX)

# Find Sandbox source files
file(GLOB_RECURSE SANDBOX_SOURCES "source/*.cpp")

# Define the executable
add_executable(Sandbox ${SANDBOX_SOURCES})

# Links only to Harmony. Raylib and spdlog are NOT accessible here.
target_link_libraries(Sandbox PRIVATE Harmony::Engine)

# Copy assets to build directory
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")