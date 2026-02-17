#pragma once
#include "ISocketServer.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>
#include <cstdio>
#include <cstring>
#include <arpa/inet.h>
#include <string>


class ServerSocket : public ISocketServer
{
    public:

	    ServerSocket();
	    ~ServerSocket();

	    void	bind_socket(unsigned long ip, int port);
	    void	listen_socket(int backlog);
	    int		get_fd() const;

    private:

	    void    test_input(unsigned long ip, int port);
	    void    init_sockaddr(int port, unsigned long ip, struct sockaddr_in *addr);
	    void	print_socket_ip(int port, unsigned long ip);
	    int		sockfd;
};
