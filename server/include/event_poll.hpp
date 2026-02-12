#pragma once

#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <cstring>
#include <mutex>
#include <stdexcept>
#include <unordered_map>
#include <vector>

class EventPoll {
   public:
    EventPoll(int max_events = 256);
    ~EventPoll();

    void addFd(int fd, uint32_t events);
    void modifyFd(int fd, uint32_t events);
    void removeFd(int fd);

    void waitEvents(int timeout_ms = -1);

    struct Event {
        int      fd;
        uint32_t events;
    };

    const std::vector<Event>& events() const;

   private:
    int                               m_max_events;
    int                               m_epoll_fd;
    int                               m_event_fd;
    std::vector<struct epoll_event>   m_epoll_events;
    std::vector<Event>                m_active_events;
    std::unordered_map<int, uint32_t> m_fd_events;

    mutable std::mutex m_mutex;

    void setNonblocking(int fd);
};