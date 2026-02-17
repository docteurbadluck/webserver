#pragma once
#include "EpollServerRunner.hpp"

class EpollServerRunner;

class InitEpoll
{
	public :
		bool initialise_epoll(EpollServerRunner &epoll_runner, std::vector<ISocketServer*> &servers);

	private :
		void	initialise_epoll_fd(int &epoll_fd);
		void	link_server_socket_to_epoll(int epoll_fd, std::vector<ISocketServer*> &servers, std::vector<int> &active_server_sockets);
		void add_listening_socket(int fd, int epoll_fd, std::vector<int>& active_server_sockets);
		void set_socket_non_blocking(int fd);
		void add_stdin_fd(int epoll_fd);
};

