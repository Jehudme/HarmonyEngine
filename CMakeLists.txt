cmake_minimum_required(VERSION 3.20)
project(HarmonyWorkspace LANGUAGES C CXX)

# Standard C++20 for the whole project
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependency Management
include(FetchContent)

# Raylib
FetchContent_Declare(raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# spdlog
FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.12.0
)

# GLM
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8
)

# Glaze
FetchContent_Declare(
        glaze
        GIT_REPOSITORY https://github.com/stephenberry/glaze.git
        GIT_TAG      v4.3.0
        GIT_SHALLOW  TRUE
)

FetchContent_MakeAvailable(raylib spdlog glm glaze)

# Add sub-projects
add_subdirectory(Harmony)
add_subdirectory(Sandbox)