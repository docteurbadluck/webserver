#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include <map>
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdexcept>

#include "IClient.hpp" 

class ConnectionManager
{
public:
    bool integrate_new_client(int client_fd,
                              std::map<int, IClient*>& clients,
                              int& client_id,
                              int epoll_fd) const;

	bool remove_client(IClient* client,
                       std::map<int, IClient*>& clients,
                       int epoll_fd) const;
private:
    bool store_client_into_map(int client_fd,
                               IClient* client,
                               std::map<int, IClient*>& clients) const;
    bool add_fd_to_epoll(int client_fd,
                         int epoll_fd) const;
    bool discard_client_from_map(int client_fd,
                                 std::map<int, IClient*>& clients) const;
    bool remove_fd_from_epoll(int client_fd,
                              int epoll_fd) const;
};

#endif 

