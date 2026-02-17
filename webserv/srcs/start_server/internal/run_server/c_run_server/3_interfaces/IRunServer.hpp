#pragma once 
#include "ISocketServer.hpp"
#include <vector>

class IRunServer
{
	public :
		virtual			~IRunServer(){};
		virtual void	run() = 0;
		virtual void	init(std::vector<ISocketServer*> servers) = 0;
};
