#include "EpollServerRunner.hpp"

void EpollServerRunner::disconnect_client(IClient* client)
{
    ConnectionManager conn_mgr;

    conn_mgr.remove_client(client, this->clients, this->epoll_fd);
}
