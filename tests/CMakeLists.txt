include(FetchContent)
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(catch2)

enable_testing()

add_executable(server_test server_test.cpp)

file(GLOB SERVER_SOURCES 
    "${CMAKE_SOURCE_DIR}/server/src/*.cpp"
)
list(FILTER SERVER_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

target_sources(server_test PRIVATE ${SERVER_SOURCES})

target_include_directories(server_test PRIVATE 
    "${CMAKE_SOURCE_DIR}/server/include"
)

target_link_libraries(server_test PRIVATE protocol socketpoll Catch2::Catch2WithMain)

#list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(server_test)