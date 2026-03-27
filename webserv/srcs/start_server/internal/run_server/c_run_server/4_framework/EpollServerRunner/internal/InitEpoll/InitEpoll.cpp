#include "InitEpoll.hpp"
#include <signal.h>

bool InitEpoll::initialise_epoll(EpollServerRunner &epoll_runner, std::vector<ISocketServer*>& servers)
{
	signal(SIGCHLD, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	initialise_epoll_fd(epoll_runner.epoll_fd);	
	link_server_socket_to_epoll(epoll_runner.epoll_fd, servers, epoll_runner.accepting_socket);
	add_stdin_fd(epoll_runner.epoll_fd);
	epoll_runner.mode_switcher = new ModeSwitcher(epoll_runner.epoll_fd);
	epoll_runner.client_id = 1;
	epoll_runner.running =1; 
	return true;
}

void InitEpoll::initialise_epoll_fd(int &epoll_fd)
{
	epoll_fd = epoll_create(100);
	if (epoll_fd == -1)
		throw std::runtime_error("epoll_creation_failed");
}

void InitEpoll::link_server_socket_to_epoll(int epoll_fd, std::vector<ISocketServer*>& servers, std::vector<int>& active_server_sockets)
{
	std::vector<ISocketServer*>::iterator it;

	for (it = servers.begin(); it != servers.end(); ++it)
	{
		add_listening_socket((*it)->get_fd(), epoll_fd, active_server_sockets);
	}
}

void InitEpoll::add_listening_socket(int fd, int epoll_fd, std::vector<int>& active_server_sockets)
{
	struct epoll_event ev;

	set_socket_non_blocking(fd);
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1)
		throw std::runtime_error("epoll_ctl add failed");
	active_server_sockets.push_back(fd);
}


void InitEpoll::set_socket_non_blocking(int fd)
{
	fcntl(fd, F_SETFL, O_NONBLOCK);
}


void InitEpoll::add_stdin_fd(int epoll_fd)
{
	struct epoll_event ev;

	ev.events = EPOLLIN;
	ev.data.fd = 0;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &ev) == -1)
		throw std::runtime_error("epoll_ctl add failed");
}

