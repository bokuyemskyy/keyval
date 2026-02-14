cmake_minimum_required(VERSION 3.20)

project(keyval LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  socketpoll
  GIT_REPOSITORY https://github.com/bokuyemskyy/socketpoll.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(socketpoll)

add_subdirectory(lib/protocol)
add_subdirectory(server)
add_subdirectory(client)

# Tests
option(BUILD_TESTING "Build tests" OFF)
include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()