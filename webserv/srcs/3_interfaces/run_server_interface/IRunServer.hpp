#pragma once 
#include "ISocketServer.hpp"
#include <vector>

class IRunServer
{
	public :
		virtual			~IRunServer(){};
		virtual void	run() = 0;
		virtual void	init_listening_socket(std::vector<ISocketServer*> servers) = 0;
};
