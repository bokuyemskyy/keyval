#pragma once

#include <cstdint>
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
    EventPoll(int max_events = 256);
    ~EventPoll();

    void addFd(int fd, PollEvent event);
    void modifyFd(int fd, PollEvent event);
    void removeFd(int fd);

    std::vector<int> wait(int timeout_ms = -1);

    struct PollEventEntry {
        int       fd;
        PollEvent events;
    };

    const std::vector<PollEventEntry>& events() const;

   private:
    int m_max_events;

    int m_poll_handle;

    std::vector<PollEventEntry> m_active_events;

    mutable std::mutex m_mutex;

    void setNonblocking(int fd);

    static uint32_t  toNative(PollEvent event);
    static PollEvent fromNative(uint32_t native);
};