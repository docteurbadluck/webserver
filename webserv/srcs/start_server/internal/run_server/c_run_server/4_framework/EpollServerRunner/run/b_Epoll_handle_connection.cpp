#include "EpollServerRunner.hpp"

void EpollServerRunner::connect_new_client(int server_fd)
{
	ConnectionManager connection_manager;
	while (true)
	{
		int client_fd = accept_connection(server_fd);
		if (client_fd == -1)
			break;
		set_socket_non_blocking(client_fd);
		if (!connection_manager.integrate_new_client(
						client_fd,
						this->clients,
						this->client_id,
						this->epoll_fd))
			continue;
		timeout_manager.update_timestamp_last_activity(clients[client_fd]);
	}
}

int EpollServerRunner::accept_connection(int server_fd)
{
	int client_fd = accept(server_fd, NULL, NULL);
	if (client_fd == -1)
		return -1;
	return client_fd;
}
