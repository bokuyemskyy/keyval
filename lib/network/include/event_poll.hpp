#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>
#include "socket.hpp"

enum PollEvent : uint8_t {
    None  = 0,
    Read  = 1 << 0,
    Write = 1 << 1,
    Error = 1 << 2
};

class EventPoll {
   public:
    struct PollEventEntry {
        socket_t       fd;
        PollEvent events;
    };

    EventPoll(int max_events = 256);
    ~EventPoll();

    EventPoll(const EventPoll&)            = delete;
    EventPoll& operator=(const EventPoll&) = delete;

    void addFd(socket_t fd, PollEvent event);
    void modifyFd(socket_t fd, PollEvent event);
    void removeFd(socket_t fd);
    void wait(int timeout_ms = -1);

    const std::vector<PollEventEntry>& events() const;

   private:
    int m_max_events;

    class Impl;
    std::unique_ptr<Impl> m_pimpl;
};