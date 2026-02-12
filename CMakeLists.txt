cmake_minimum_required(VERSION 3.20)

project(keyval LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build unit tests" ON)

add_subdirectory(lib/poll)
add_subdirectory(lib/protocol)
add_subdirectory(server)
add_subdirectory(client)

if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
    )
    FetchContent_MakeAvailable(catch2)
    enable_testing()
    add_subdirectory(tests)
endif()



# ___ Server ___
#file(GLOB SERVER_SRCS "${SRC_DIR}/server/*.cpp")
#add_executable(keyval-server ${SERVER_SRCS})
#set_target_properties(keyval-server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

# ___ Client ___
#file(GLOB CLIENT_SRCS "${SRC_DIR}/client/*.cpp")
#add_executable(keyval-client ${CLIENT_SRCS})
#set_target_properties(keyval-client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

# ___ Shared ___
#file(GLOB COMMON_SRCS "${SRC_DIR}/common/*.cpp")
#add_library(keyval-common ${COMMON_SRCS})
#target_link_libraries(keyval-server PRIVATE keyval-common)
#target_link_libraries(keyval-client PRIVATE keyval-common)

# ___ Install ___
#install(TARGETS keyval-server keyval-client DESTINATION bin)
