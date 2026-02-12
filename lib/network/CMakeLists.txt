
if(APPLE)
    set(POLL_SRC "src/poll_kqueue.cpp")
elseif(WIN32)
    set(POLL_SRC "src/poll_winsock.cpp")
    list(APPEND EXTRA_LIBS ws2_32)
else()
    set(POLL_SRC "src/poll_epoll.cpp")
endif()

add_library(network STATIC ${POLL_SRC})

target_include_directories(network PUBLIC include)