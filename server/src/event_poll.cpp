#include "event_poll.hpp"

EventPoll::EventPoll(int max_events)
    : m_max_events(max_events),
      m_epoll_fd(epoll_create1(0)),
      m_event_fd(eventfd(0, EFD_NONBLOCK)),
      m_epoll_events(max_events)
{
    if (m_epoll_fd == -1)
        throw std::runtime_error(strerror(errno));
    if (m_event_fd == -1)
        throw std::runtime_error(strerror(errno));

    addFd(m_event_fd, EPOLLIN);
}

EventPoll::~EventPoll()
{
    close(m_epoll_fd);
    close(m_event_fd);
}

void EventPoll::setNonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        throw std::runtime_error(strerror(errno));
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        throw std::runtime_error(strerror(errno));
}

void EventPoll::addFd(int fd, uint32_t events)
{
    setNonblocking(fd);

    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;

    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1)
        throw std::runtime_error(strerror(errno));
    m_fd_events[fd] = events;
}

void EventPoll::modifyFd(int fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;

    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1)
        throw std::runtime_error(strerror(errno));
    m_fd_events[fd] = events;
}

void EventPoll::removeFd(int fd)
{
    epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
    m_fd_events.erase(fd);
    close(fd);
}

void EventPoll::waitEvents(int timeout_ms)
{
    int n = epoll_wait(m_epoll_fd, m_epoll_events.data(), m_max_events, timeout_ms);
    if (n == -1 && errno != EINTR)
        throw std::runtime_error(strerror(errno));

    m_active_events.clear();
    for (int i = 0; i < n; i++)
    {
        m_active_events.push_back({m_epoll_events[i].data.fd, m_epoll_events[i].events});
    }
}

const std::vector<EventPoll::Event> &EventPoll::events() const
{
    return m_active_events;
}