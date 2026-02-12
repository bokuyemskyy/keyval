#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

enum PollEvent : uint8_t {
    None  = 0,
    Read  = 1 << 0,
    Write = 1 << 1,
    Error = 1 << 2
};

class EventPoll {
   public:
    struct PollEventEntry {
        int       fd;
        PollEvent events;
    };

    EventPoll(int max_events = 256);
    ~EventPoll();

    EventPoll(const EventPoll&)            = delete;
    EventPoll& operator=(const EventPoll&) = delete;

    void addFd(int fd, PollEvent event);
    void modifyFd(int fd, PollEvent event);
    void removeFd(int fd);
    void wait(int timeout_ms = -1);

    const std::vector<PollEventEntry>& events() const;

   private:
    class Impl;
    std::unique_ptr<Impl> pimpl;

    void setNonblocking(int fd);
};