cmake_minimum_required(VERSION 3.16)
project(keyval LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23 CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

option(BUILD_TESTS "Build unit tests" ON)

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)

if(BUILD_TESTS)
    add_subdirectory(third_party/catch2)
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
