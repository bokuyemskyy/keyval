#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

#include <cstring>
#include <mutex>
#include <stdexcept>

#include "event_poll.hpp"

class EventPoll::Impl {
   public:
    socket_t                             m_epoll_fd;
    std::vector<struct epoll_event> m_kernel_events;
    std::vector<PollEventEntry>     m_active_events;
    std::mutex                      m_mutex;

    Impl(int max_events) : m_epoll_fd(epoll_create1(0)) {
        if (m_epoll_fd == INVALID_SOCKET_FD) throw std::runtime_error(strerror(errno));

        m_kernel_events.resize(max_events);
    }

    ~Impl() {
        if (m_epoll_fd != INVALID_SOCKET_FD) close(m_epoll_fd);
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

EventPoll::EventPoll(int max_events) : m_pimpl(std::make_unique<Impl>(max_events)), m_max_events(max_events) {}
EventPoll::~EventPoll() = default;


void EventPoll::addFd(socket_t fd, PollEvent event) {
    std::lock_guard lock(m_pimpl->m_mutex);

    struct epoll_event ev{};

    ev.events  = Impl::toNative(event);
    ev.data.fd = fd;

    if (epoll_ctl(m_pimpl->m_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) throw std::runtime_error(strerror(errno));
}

void EventPoll::modifyFd(socket_t fd, PollEvent event) {
    std::lock_guard lock(m_pimpl->m_mutex);

    struct epoll_event ev{};

    ev.events  = Impl::toNative(event);
    ev.data.fd = fd;

    if (epoll_ctl(m_pimpl->m_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) throw std::runtime_error(strerror(errno));
}

void EventPoll::removeFd(socket_t fd) {
    std::lock_guard lock(m_pimpl->m_mutex);
    epoll_ctl(m_pimpl->m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
}

void EventPoll::wait(int timeout_ms) {
    int n = epoll_wait(m_pimpl->m_epoll_fd, m_pimpl->m_kernel_events.data(), m_max_events, timeout_ms);

    if (n == -1) {
        if (errno == EINTR) return;
        throw std::runtime_error(strerror(errno));
    }

    std::lock_guard lock(m_pimpl->m_mutex);
    m_pimpl->m_active_events.clear();
    for (int i = 0; i < n; i++) {
        m_pimpl->m_active_events.push_back(
            {m_pimpl->m_kernel_events[i].data.fd, Impl::fromNative(m_pimpl->m_kernel_events[i].events)});
    }
}

const std::vector<EventPoll::PollEventEntry>& EventPoll::events() const { return m_pimpl->m_active_events; }