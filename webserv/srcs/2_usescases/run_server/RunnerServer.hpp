#include "IRunServer.hpp"
#include "ISocketServer.hpp"
#include "server_rules.hpp"
#include <vector>

class RunnerServer
{
	public:
	RunnerServer(std::vector<ISocketServer*> servers, IRunServer& runner);
	void							start_the_server();
	private :
		std::vector<ISocketServer*>	servers;
		IRunServer&					runner;
};
