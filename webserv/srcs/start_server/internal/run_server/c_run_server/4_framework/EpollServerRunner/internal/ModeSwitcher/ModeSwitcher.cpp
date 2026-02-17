#include "ModeSwitcher.hpp"
#include "IClient.hpp"

ModeSwitcher::ModeSwitcher(int epoll_fd)
    : epoll_fd(epoll_fd)
{
}

void ModeSwitcher::apply_events(int fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;

    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
}

void ModeSwitcher::switch_to_read_mode(IClient *client)
{
    apply_events(client->get_fd(), EPOLLIN);
    client->stream.active = false;
}

void ModeSwitcher::switch_to_write_mode(IClient *client)
{
    apply_events(client->get_fd(), EPOLLOUT);
    client->stream.active = true;
}


bool ModeSwitcher::is_read_event(uint32_t events)
{
    return (events & EPOLLIN);
}

bool ModeSwitcher::is_write_event(uint32_t events)
{
    return (events & EPOLLOUT);
}
