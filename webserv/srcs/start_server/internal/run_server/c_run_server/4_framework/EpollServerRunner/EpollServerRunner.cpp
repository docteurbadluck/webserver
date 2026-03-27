#include "EpollServerRunner.hpp"

EpollServerRunner::EpollServerRunner(const t_server_rules &server_rules, SessionHandlerUC &session_handler, RequestHandlerUC &request_handler) : server_rules(server_rules), session_handler(session_handler), request_handler(request_handler)
{
	stream_in.set_max_body_size(server_rules.max_size_request_body);
}

EpollServerRunner::~EpollServerRunner()
{
	for (std::map<int, IClient*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		delete it->second;
	}
	this->clients.erase(this->clients.begin(), this->clients.end());
	delete this->mode_switcher;
	close(this->epoll_fd);
}
