#pragma once
#include "ISocketServer.hpp"

class ISocketFactory
{
	public:
	virtual ~ISocketFactory() {}
	virtual ISocketServer* create_server_socket() = 0;
};