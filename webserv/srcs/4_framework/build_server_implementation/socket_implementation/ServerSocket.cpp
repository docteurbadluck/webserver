#include "ServerSocket.hpp"
#include <iostream>

ServerSocket::ServerSocket() : sockfd(-1)
{
	int	opt;

	this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->sockfd < 0)
		throw std::runtime_error("failed to create socket");
	opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("failed to set option");

}

ServerSocket::~ServerSocket()
{
	if (this->sockfd >= 0)
		close(this->sockfd);
}

void ServerSocket::listen_socket(int backlog)
{
	if (listen(this->sockfd, backlog) < 0)
		throw std::runtime_error("Listen failed");
}

int ServerSocket::get_fd() const
{
	return (this->sockfd);
}

