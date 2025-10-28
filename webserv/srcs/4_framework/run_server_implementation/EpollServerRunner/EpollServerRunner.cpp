#include "EpollServerRunner.hpp"

EpollServerRunner::EpollServerRunner(const t_server_rules &server_rules, SessionHandlerUC &session_handler, RequestHandlerUC &request_handler) : server_rules(server_rules), session_handler(session_handler), request_handler(request_handler)
{
	this->epoll_fd = epoll_create(100);
	if (this->epoll_fd == -1)
		throw std::runtime_error("epoll_create failed");
	this->client_id = 1;
	this->running = true;
}

EpollServerRunner::~EpollServerRunner()
{
	for (std::map<int, IClient*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		delete it->second;
	}
	this->clients.erase(this->clients.begin(), this->clients.end());
	
	close(this->epoll_fd);
}
