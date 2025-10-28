#include "EpollServerRunner.hpp"


void EpollServerRunner::handle_new_connection(int server_fd)
{
	while (true)
	{
		int client_fd = accept_connection(server_fd);
		if (client_fd == -1)
			break;
		set_socket_non_blocking(client_fd);
		store_client_in_map(client_fd);
		add_client_fd_to_epoll(client_fd);
		this->clients[client_fd]->update_last_activity();
	}
	std::cout << "New client connected." << std::endl;
}

int EpollServerRunner::accept_connection(int server_fd)
{
	int client_fd = accept(server_fd, NULL, NULL);
	if (client_fd == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return -1;
		throw std::runtime_error("accept failed");
	}
	return client_fd;
}

void EpollServerRunner::store_client_in_map(int client_fd)
{
	if (this->clients.find(client_fd) != this->clients.end())
		std::cerr << "Warning: client_fd already exists: " << client_fd << std::endl;
	IClient* new_client = new Client(client_fd, this->client_id++);
	this->clients[client_fd] = new_client;
}

void EpollServerRunner::add_client_fd_to_epoll(int client_fd)
{
	struct epoll_event ev;

	ev.events = EPOLLIN;
	ev.data.fd = client_fd;
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
	{
		delete clients[client_fd];
		clients.erase(client_fd);
		close(client_fd);
	}
}

void EpollServerRunner::disconnect_client(IClient *client)
{
	remove_client_fd_from_epoll(client->get_fd());
	std::cout << "Client disconnected: fd " << client->get_fd() << std::endl;
	discard_client_from_map(client->get_fd());
}

void EpollServerRunner::remove_client_fd_from_epoll(int fd)
{
	epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

void EpollServerRunner::discard_client_from_map(int fd)
{
	std::map<int, IClient*>::iterator it = this->clients.find(fd);
	if (it != this->clients.end())
	{
		close(it->second->get_fd());
		delete it->second;
		this->clients.erase(it);
	}
}
