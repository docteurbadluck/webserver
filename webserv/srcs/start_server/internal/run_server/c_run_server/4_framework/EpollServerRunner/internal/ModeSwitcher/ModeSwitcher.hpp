#pragma once

#include <sys/epoll.h>

class IClient;

class ModeSwitcher
{
public:
    ModeSwitcher(int epoll_fd);

    void switch_to_read_mode(IClient *client);
    void switch_to_write_mode(IClient *client);

    bool is_read_event(uint32_t events);
    bool is_write_event(uint32_t events);

private:
    int epoll_fd;
    void apply_events(int fd, uint32_t events);
};
