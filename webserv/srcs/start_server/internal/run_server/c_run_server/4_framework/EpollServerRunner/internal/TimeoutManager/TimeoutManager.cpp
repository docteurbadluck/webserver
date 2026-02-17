#include "TimeoutManager.hpp"

int TimeoutManager::check_clients_timeout(
    std::map<int, IClient*> &clients,
    const t_server_rules &rules, const int &epoll_fd)
{
    std::vector<IClient*> timed_out =
        list_clients_timed_out(clients, rules);

    return disconnect_timed_out_clients(timed_out, clients,epoll_fd);
}


std::vector<IClient*> TimeoutManager::list_clients_timed_out(
    std::map<int, IClient*> &clients,
    const t_server_rules &rules)
{
    std::vector<IClient*> timed_out;
    std::time_t now = std::time(NULL);

    for (std::map<int, IClient*>::iterator it = clients.begin();
         it != clients.end(); ++it)
    {
        IClient *c = it->second;

        if (now > c->last_activity + rules.client_timeout_in_second)
            timed_out.push_back(c);
    }
    return timed_out;
}


int TimeoutManager::disconnect_timed_out_clients(
    std::vector<IClient*> &clients_to_disconnect, std::map<int, IClient *> &clients, const int &epoll_fd)
{
	ConnectionManager connection_manager;

    for (size_t i = 0; i < clients_to_disconnect.size(); ++i)
    {
        send_timeout_response(clients_to_disconnect[i]);
		connection_manager.remove_client(clients_to_disconnect[i], clients, epoll_fd);
		
    }
    return clients_to_disconnect.size();
}

void TimeoutManager::send_timeout_response(IClient *c)
{
    const char *response =
        "HTTP/1.1 408 Request Timeout\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n";

    send(c->get_fd(), response, strlen(response), 0);
}

void TimeoutManager::update_timestamp_last_activity(IClient *client)
{
    client->last_activity = std::time(NULL);
}

