#include "SocketFactory.hpp"

ISocketServer* SocketFactory::create_server_socket()
{
	return new ServerSocket();
}
