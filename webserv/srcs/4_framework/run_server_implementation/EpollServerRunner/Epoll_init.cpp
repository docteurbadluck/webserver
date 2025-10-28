#include "EpollServerRunner.hpp"

void EpollServerRunner::init_listening_socket(std::vector<ISocketServer*> servers)
{
	std::vector<ISocketServer*>::iterator it;

	for (it = servers.begin(); it != servers.end(); ++it)
	{
		add_listening_socket((*it)->get_fd());
	}
	add_stdin_fd();
	
}

void EpollServerRunner::add_listening_socket(int fd)
{
	struct epoll_event ev;

	set_socket_non_blocking(fd);
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1)
		throw std::runtime_error("epoll_ctl add failed");
	accepting_socket.push_back(fd);
}


void EpollServerRunner::set_socket_non_blocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) 
		flags = 0;
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


void EpollServerRunner::add_stdin_fd()
{
	struct epoll_event ev;

	ev.events = EPOLLIN;
	ev.data.fd = 0;
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, 0, &ev) == -1)
		throw std::runtime_error("epoll_ctl add failed");
}