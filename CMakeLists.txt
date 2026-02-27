cmake_minimum_required(VERSION 3.20)

# Still good to keep this as a safety net for any deep nested dependencies
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(HarmonyWorkspace LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# --- Raylib (Latest Master) ---
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
FetchContent_Declare(raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        master
)

# --- GLM (1.0.1 - Latest Stable) ---
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
)

# --- spdlog (v1.15.0 - Latest Stable) ---
FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.15.0
)

# --- Glaze (v4.3.0 - Latest Stable) ---
FetchContent_Declare(glaze
        GIT_REPOSITORY https://github.com/stephenberry/glaze.git
        GIT_TAG        v4.3.0
        GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(raylib glm spdlog glaze)

add_subdirectory(Harmony)
add_subdirectory(Sandbox)