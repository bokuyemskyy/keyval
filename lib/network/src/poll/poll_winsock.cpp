#include <winsock2.h>
#include <ws2tcpip.h>

#include <cstring>
#include <mutex>
#include <stdexcept>
#include <unordered_map>

#include "event_poll.hpp"


class EventPoll::Impl {
   public:
    std::vector<WSAPOLLFD>                      m_poll_fds;
    std::unordered_map<socket_t, PollEvent>     m_fd_map;
    std::vector<PollEventEntry>     m_active_events;
    std::mutex                      m_mutex;
    int                                         m_max_events;

    Impl(int max_events) : m_max_events(max_events) {
        m_poll_fds.reserve(max_events);
    }


    ~Impl() {
        WSACleanup();
    }

    static short toNative(PollEvent event) {
        short native = 0;
        if (event & PollEvent::Read) native |= POLLRDNORM | POLLRDBAND;
        if (event & PollEvent::Write) native |= POLLWRNORM;
        if (event & PollEvent::Error) native |= POLLERR | POLLHUP;
        return native;
    }

    static PollEvent fromNative(short native) {
        uint8_t res = PollEvent::None;
        if (native & (POLLRDNORM | POLLRDBAND)) res |= PollEvent::Read;
        if (native & POLLWRNORM) res |= PollEvent::Write;
        if (native & (POLLERR | POLLHUP | POLLNVAL)) res |= PollEvent::Error;
        return static_cast<PollEvent>(res);
    }
    
    void rebuildPollArray() {
        m_poll_fds.clear();
        for (const auto& entry : m_fd_map) {
            WSAPOLLFD pfd{};
            pfd.fd = entry.first;
            pfd.events = toNative(entry.second);
            pfd.revents = 0;
            m_poll_fds.push_back(pfd);
        }
    }
};

EventPoll::EventPoll(int max_events) : m_pimpl(std::make_unique<Impl>(max_events)), m_max_events(max_events) {}
EventPoll::~EventPoll() = default;


void EventPoll::addFd(socket_t fd, PollEvent event) {
    std::lock_guard lock(m_pimpl->m_mutex);

    if (m_pimpl->m_fd_map.find(fd) != m_pimpl->m_fd_map.end()) {
        throw std::runtime_error("File descriptor already exists");
    }

    m_pimpl->m_fd_map[fd] = event;
    m_pimpl->rebuildPollArray();
}

void EventPoll::modifyFd(socket_t fd, PollEvent event) {
    std::lock_guard lock(m_pimpl->m_mutex);

    auto it = m_pimpl->m_fd_map.find(fd);
    if (it == m_pimpl->m_fd_map.end()){
        throw std::runtime_error("File descriptor not found");
    }

    it->second = event;
    m_pimpl->rebuildPollArray();
}

void EventPoll::removeFd(socket_t fd) {
    std::lock_guard lock(m_pimpl->m_mutex);

    m_pimpl->m_fd_map.erase(fd);
    m_pimpl->rebuildPollArray();
}

void EventPoll::wait(int timeout_ms) {
    std::vector<WSAPOLLFD> poll_fds_copy;
    
    {
        std::lock_guard lock(m_pimpl->m_mutex);
        if (m_pimpl->m_poll_fds.empty()) {
            m_pimpl->m_active_events.clear();
            return;
        }
        poll_fds_copy = m_pimpl->m_poll_fds;
    }

    int n = WSAPoll(poll_fds_copy.data(), 
                    static_cast<ULONG>(poll_fds_copy.size()), 
                    timeout_ms);

    if (n == SOCKET_ERROR) {
        int error = WSAGetLastError();
        if (error == WSAEINTR) return;
        throw std::runtime_error("WSAPoll failed: " + std::to_string(error));
    }

    std::lock_guard lock(m_pimpl->m_mutex);
    m_pimpl->m_active_events.clear();
    
    for (const auto& pfd : poll_fds_copy) {
        if (pfd.revents != 0) {
            m_pimpl->m_active_events.push_back({
                pfd.fd, 
                Impl::fromNative(pfd.revents)
            });
        }
    }
}

const std::vector<EventPoll::PollEventEntry>& EventPoll::events() const { 
    return m_pimpl->m_active_events; 
}