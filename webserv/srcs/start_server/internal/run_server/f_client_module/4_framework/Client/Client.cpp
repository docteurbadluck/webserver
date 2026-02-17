#include "Client.hpp"
#include <iostream>
#include <unistd.h>

Client::Client(int fd, int id_client) : fd(fd), id_client(id_client)
{
	this->stream.active = false;
	std::cout << "Client constructed: fd=" << fd << ", id=" << id_client << std::endl;
}

Client::~Client()
{
	close(this->fd);
}

int Client::get_fd() const
{
	return fd;
}
