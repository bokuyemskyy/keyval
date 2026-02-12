#include <sys/epoll.h>
#include <sys/eventfd.h>

#include "event_poll.hpp"

EventPoll::EventPoll(int max_events) : m_max_events(max_events), m_active_events(max_events) {
    m_poll_handle = epoll_create1(0);
    if (m_poll_handle == -1) throw std::runtime_error(strerror(errno));

    int m_event_fd = eventfd(0, EFD_NONBLOCK);
    if (m_event_fd == -1) {
        close(m_poll_handle);
        throw std::runtime_error(strerror(errno));
    }

    addFd(m_event_fd, PollEvent::Read);
}

EventPoll::~EventPoll() {
    close(m_epoll_fd);
    // to close the event fd
}

void EventPoll::setNonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) throw std::runtime_error(strerror(errno));
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) throw std::runtime_error(strerror(errno));
}

void EventPoll::addFd(int fd, PollEvent event) {
    std::unique_lock lock(m_mutex);
    setNonblocking(fd);

    struct epoll_event ev;
    ev.events  = toNative(event);
    ev.data.fd = fd;

    if (epoll_ctl(m_poll_handle, EPOLL_CTL_ADD, fd, &ev) == -1) throw std::runtime_error(strerror(errno));
}

void EventPoll::modifyFd(int fd, PollEvent event) {
    std::unique_lock   lock(m_mutex);
    struct epoll_event ev;
    ev.events  = toNative(event);
    ev.data.fd = fd;

    if (epoll_ctl(m_poll_handle, EPOLL_CTL_MOD, fd, &ev) == -1) throw std::runtime_error(strerror(errno));
}

void EventPoll::removeFd(int fd) {
    std::unique_lock lock(m_mutex);
    epoll_ctl(m_poll_handle, EPOLL_CTL_DEL, fd, nullptr);
}

void EventPoll::wait(int timeout_ms) {
    int n = epoll_wait(m_epoll_handle, m_epoll_events.data(), m_max_events, timeout_ms);
    if (n == -1 && errno != EINTR) throw std::runtime_error(strerror(errno));

    std::unique_lock lock(m_mutex);
    m_active_events.clear();
    for (int i = 0; i < n; i++) {
        m_active_events.push_back({m_epoll_events[i].data.fd, m_epoll_events[i].events});
    }
}

const std::vector<EventPoll::Event>& EventPoll::events() const { return m_active_events; }

uint32_t EventPoll::toNative(PollEvent event) {
    uint32_t native = 0;
    if (event & PollEvent::Read) native |= EPOLLIN;
    if (event & PollEvent::Write) native |= EPOLLOUT;
    if (event & PollEvent::Error) native |= EPOLLERR;
    return native;
}

PollEvent EventPoll::fromNative(uint32_t native) {
    uint8_t res = PollEvent::None;
    if (native & EPOLLIN) res |= PollEvent::Read;
    if (native & EPOLLOUT) res |= PollEvent::Write;
    if (native & (EPOLLERR | EPOLLHUP)) res |= PollEvent::Error;
    return static_cast<PollEvent>(res);
}