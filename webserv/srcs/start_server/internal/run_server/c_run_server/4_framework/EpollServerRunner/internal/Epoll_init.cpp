#include "EpollServerRunner.hpp"

void EpollServerRunner::init(std::vector<ISocketServer*> servers)
{
	InitEpoll init_epoll;

	init_epoll.initialise_epoll(*this, servers);
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

void EpollServerRunner::turn_off()
{
	this->running = false;
}
