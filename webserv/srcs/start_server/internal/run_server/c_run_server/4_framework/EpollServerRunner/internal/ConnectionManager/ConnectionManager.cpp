#include "ConnectionManager.hpp"

#include "ConnectionManager.hpp"
#include "Client.hpp" 

bool ConnectionManager::integrate_new_client(int client_fd,
                                             std::map<int, IClient*>& clients,
                                             int& client_id,
                                             int epoll_fd) const
{
    IClient* new_client = new Client(client_fd, client_id++);
    if (!store_client_into_map(client_fd, new_client, clients))
    {
        delete new_client;
        return false;
    }

    if (!add_fd_to_epoll(client_fd, epoll_fd))
    {
        discard_client_from_map(client_fd, clients);
        delete new_client;
        close(client_fd);
        return false;
    }

    return true;
}

bool ConnectionManager::store_client_into_map(int client_fd,
                                              IClient* client,
                                              std::map<int, IClient*>& clients) const
{
    if (clients.find(client_fd) != clients.end())
        std::cerr << "Warning: client_fd already exists: "
                  << client_fd << std::endl;

    clients[client_fd] = client;
    return true;
}

bool ConnectionManager::add_fd_to_epoll(int client_fd,
                                        int epoll_fd) const
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = client_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
    {
        std::cerr<<"epoll_ctl ADD failed" << std::endl;
        return false;
    }
    return true;
}

bool ConnectionManager::remove_client(IClient* client,
                                     std::map<int, IClient*>& clients,
                                     int epoll_fd) const
{
    if (!client)
        return false;

    remove_fd_from_epoll(client->get_fd(), epoll_fd);
    discard_client_from_map(client->get_fd(), clients);

    return true;
}

bool ConnectionManager::remove_fd_from_epoll(int client_fd,
                                             int epoll_fd) const
{
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL) == -1)
    {
        std::cerr <<"epoll_ctl DEL failed\n";
        return false;
    }
    return true;
}

bool ConnectionManager::discard_client_from_map(int client_fd,
                                                std::map<int, IClient*>& clients) const
{
    std::map<int, IClient*>::iterator it = clients.find(client_fd);
    if (it != clients.end())
    {
        close(it->second->get_fd());
        delete it->second;
        clients.erase(it);
    }
    return true;
}
