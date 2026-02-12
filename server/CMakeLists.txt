file(GLOB SRCS
    src/*.cpp
)
add_executable(server ${SRCS})

target_include_directories(server PRIVATE include)

target_link_libraries(server PRIVATE protocol)
target_link_libraries(server PRIVATE network)