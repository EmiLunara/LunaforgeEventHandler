cmake_minimum_required(VERSION 3.25)

project(ProjectEventBus
    VERSION 0.1.0
    LANGUAGES CXX
)


# -----------------------
# External libraries
# -----------------------

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
)

FetchContent_MakeAvailable(Catch2)

# -----------------------
# Main executables
# -----------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(ProjectEventBus
    src/Subscription.cpp
)

add_library(Lunaforge::EventHandler ALIAS ProjectEventBus)

target_include_directories(ProjectEventBus
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(EventBusExample
    examples/BasicExample.cpp
)

target_link_libraries(EventBusExample
    PRIVATE
        Lunaforge::EventHandler
)

# -----------------------
# Tests
# -----------------------


enable_testing()

add_executable(EventBusTests
    tests/EventBusTests.cpp
)

target_link_libraries(EventBusTests
    PRIVATE
        Lunaforge::EventHandler
        Catch2::Catch2WithMain
)

include(CTest)
include(Catch)

catch_discover_tests(EventBusTests)