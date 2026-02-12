#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

#include <cstring>
#include <mutex>
#include <stdexcept>

#include "event_poll.hpp"

class EventPoll::Impl {
   public:
    int                             m_epoll_fd;
    int                             m_max_events;
    std::vector<struct epoll_event> m_kernel_events;
    std::vector<PollEventEntry>     m_active_events;
    std::mutex                      m_mutex;

    Impl(int max_events) : m_epoll_fd(epoll_create1(0)), m_max_events(max_events) {
        if (m_epoll_fd == -1) throw std::runtime_error(strerror(errno));

        m_kernel_events.resize(max_events);
    }

    ~Impl() {
        if (m_epoll_fd != -1) close(m_epoll_fd);
    }

    static uint32_t toNative(PollEvent event) {
        uint32_t native = 0;
        if (event & PollEvent::Read) native |= EPOLLIN;
        if (event & PollEvent::Write) native |= EPOLLOUT;
        if (event & PollEvent::Error) native |= (EPOLLERR | EPOLLHUP);
        return native;
    }

    static PollEvent fromNative(uint32_t native) {
        uint8_t res = PollEvent::None;
        if (native & EPOLLIN) res |= PollEvent::Read;
        if (native & EPOLLOUT) res |= PollEvent::Write;
        if (native & (EPOLLERR | EPOLLHUP)) res |= PollEvent::Error;
        return static_cast<PollEvent>(res);
    }
};

EventPoll::EventPoll(int max_events) : pimpl(std::make_unique<Impl>(max_events)) {}
EventPoll::~EventPoll() = default;

void EventPoll::setNonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) throw std::runtime_error(strerror(errno));
}

void EventPoll::addFd(int fd, PollEvent event) {
    std::lock_guard lock(pimpl->m_mutex);

    setNonblocking(fd);

    struct epoll_event ev{};

    ev.events  = Impl::toNative(event);
    ev.data.fd = fd;

    if (epoll_ctl(pimpl->m_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) throw std::runtime_error(strerror(errno));
}

void EventPoll::modifyFd(int fd, PollEvent event) {
    std::lock_guard lock(pimpl->m_mutex);

    struct epoll_event ev{};

    ev.events  = Impl::toNative(event);
    ev.data.fd = fd;

    if (epoll_ctl(pimpl->m_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) throw std::runtime_error(strerror(errno));
}

void EventPoll::removeFd(int fd) {
    std::lock_guard lock(pimpl->m_mutex);
    epoll_ctl(pimpl->m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
}

void EventPoll::wait(int timeout_ms) {
    int n = epoll_wait(pimpl->m_epoll_fd, pimpl->m_kernel_events.data(), pimpl->m_max_events, timeout_ms);

    if (n == -1) {
        if (errno == EINTR) return;
        throw std::runtime_error(strerror(errno));
    }

    std::lock_guard lock(pimpl->m_mutex);
    pimpl->m_active_events.clear();
    for (int i = 0; i < n; i++) {
        pimpl->m_active_events.push_back(
            {pimpl->m_kernel_events[i].data.fd, Impl::fromNative(pimpl->m_kernel_events[i].events)});
    }
}

const std::vector<EventPoll::PollEventEntry>& EventPoll::events() const { return pimpl->m_active_events; }