#include "EpollServerRunner.hpp"

void EpollServerRunner::disconect_expired_client()
{
	std::time_t now = std::time(NULL);
	std::vector<IClient*> to_disconnect;

	for (std::map<int, IClient*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		IClient* c = it->second;
		if (c->is_timed_out(now, this->server_rules.client_timeout_in_second))
		{
			send_timeout_to_client(c);
			to_disconnect.push_back(c);
		}
	}
	for (std::vector<IClient*>::iterator it = to_disconnect.begin(); it != to_disconnect.end(); ++it)
	{
		disconnect_client(*it);
	}
}

void EpollServerRunner::send_timeout_to_client(IClient *c)
{
	const char *response =
		"HTTP/1.1 408 Request Timeout\r\n"
		"Content-Length: 0\r\n"
		"Connection: close\r\n"
		"\r\n";
	send(c->get_fd(), response, strlen(response), 0);
}