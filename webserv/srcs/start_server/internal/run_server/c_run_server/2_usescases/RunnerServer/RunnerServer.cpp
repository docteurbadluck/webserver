#include "RunnerServer.hpp"
#include <iostream>

RunnerServer::RunnerServer(std::vector<ISocketServer*> servers, IRunServer& runner) : servers(servers), runner(runner)
{

}

void RunnerServer::start_the_server()
{
	std::cout << "start the server succesfully." <<std::endl;
	runner.init(this->servers);
	runner.run();
}
