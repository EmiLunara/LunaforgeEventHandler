cmake_minimum_required(VERSION 3.25)

project(ProjectEventBus
    VERSION 0.1.0
    LANGUAGES CXX
)

include(FetchContent)

# -----------------------
# Main executables
# -----------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(ProjectEventBus
    src/main.cpp

   
)

add_dependencies(
    ProjectEventBus
)

target_include_directories(ProjectEventBus
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)