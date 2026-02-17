#include "ServerBuilder.hpp"


ServerBuilder::ServerBuilder(ISocketFactory &socket_factory): factory(socket_factory)
{}

void	ServerBuilder::build_server(const t_server_rules &rules)
{
	std::vector<std::pair<unsigned long, int> >::const_iterator it;

	it = rules.ip_port_vector.begin();
	while (it != rules.ip_port_vector.end())
	{
		ISocketServer* server_socket = this->factory.create_server_socket();
        
        try
        {
		    server_socket->bind_socket(it->first, it->second);
		    server_socket->listen_socket(rules.backlog);
        }
        catch( const std::exception & e)
        {
            delete server_socket;
            throw;
        }
		servers.push_back(server_socket);
		++it;
	}
}

ServerBuilder::~ServerBuilder()
{
	std::vector<ISocketServer*>::const_iterator it;

	it = this->servers.begin();
	while (it != this->servers.end())
	{
		delete *it;
		++it;
	}
}

std::vector<ISocketServer*> ServerBuilder::get_servers()
{
	return (this->servers);
}
