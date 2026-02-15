file(GLOB SRCS
    src/*.cpp
)

add_executable(keyval-server ${SRCS})

target_include_directories(keyval-server PRIVATE include)

target_link_libraries(keyval-server PRIVATE protocol socketpoll)