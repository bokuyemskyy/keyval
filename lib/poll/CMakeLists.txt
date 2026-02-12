
if(APPLE)
    set(POLL_BACKEND "src/poll_kqueue.cpp")
elseif(WIN32)
    set(POLL_BACKEND "src/poll_winsock.cpp")
    list(APPEND EXTRA_LIBS ws2_32)
else()
    set(POLL_BACKEND "src/poll_epoll.cpp")
endif()

add_library(polling STATIC src/poll_epoll.cpp)

target_include_directories(polling PUBLIC include)