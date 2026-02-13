if(WIN32)
    set(POLL_SRC "src/poll/poll_winsock.cpp")
    list(APPEND EXTRA_LIBS ws2_32)
    set(SOCKET_SRC "src/socket/socket_win.cpp")
else()
    if(APPLE)
        set(POLL_SRC "src/poll/poll_kqueue.cpp")
    else()
        set(POLL_SRC "src/poll/poll_epoll.cpp")
    endif()

    set(SOCKET_SRC "src/socket/socket_posix.cpp")
endif()

add_library(network STATIC ${POLL_SRC} ${SOCKET_SRC})

target_include_directories(network PUBLIC include)
target_link_libraries(network PUBLIC ${EXTRA_LIBS})
