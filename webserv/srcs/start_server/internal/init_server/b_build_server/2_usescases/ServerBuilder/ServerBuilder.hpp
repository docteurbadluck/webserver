#pragma once
#include "server_rules.hpp"
#include "ISocketServer.hpp"
#include "ISocketFactory.hpp"
#include <vector>


class ServerBuilder
{
	public  :
		explicit					ServerBuilder(ISocketFactory &socket_factory);
		void						build_server(const t_server_rules &rules);
		std::vector<ISocketServer*>	get_servers();
		~ServerBuilder();
	private :
		ISocketFactory 				&factory;
		std::vector<ISocketServer*>	servers;
};
