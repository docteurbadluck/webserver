#include "EpollServerRunner.hpp"

void EpollServerRunner::disconnect_client(IClient* client)
{
    ConnectionManager conn_mgr;

    conn_mgr.remove_client(client, this->clients, this->epoll_fd);
}

// sockets tools 
void EpollServerRunner::set_socket_non_blocking(int fd)
{
	fcntl(fd, F_SETFL, O_NONBLOCK);
}

bool EpollServerRunner::is_new_client(int fd) const
{
    return (std::find(accepting_socket.begin(), accepting_socket.end(),
        fd) != accepting_socket.end());
}

bool EpollServerRunner::is_keyboard_command(int fd)
{
    if (fd == 0)
    {
        return true;
    }
    return false;
}
