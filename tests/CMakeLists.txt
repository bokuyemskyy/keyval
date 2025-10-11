add_executable(server_test server_test.cpp)

target_link_libraries(server_test PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(server_test)