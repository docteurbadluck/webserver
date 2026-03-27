#include "start_server_api.hpp"

int START_SERVER(char **argv)
{
    t_server_rules  server_rules;
    ServerBuilder   *server_builder = NULL;
    SocketFactory   socket_factory;

    if (init_rules_for_the_server(server_rules, argv[1]))
        return 1;
    try
    {
        server_builder = build_the_server(server_rules, socket_factory);
        run_the_server(server_builder, server_rules);
        delete server_builder;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        delete server_builder;
        return 1;
    }
    return (0);
}

int init_rules_for_the_server(t_server_rules &server_rules, char *pathfile)
{
	try 
	{
		ConfigReader config((std::string(pathfile)));
		RulesInitialiser rules_initialiser(config);

		rules_initialiser.init_server_rules(server_rules);
	}
	catch (const std::exception & e)
	{
		std::cerr << "error found into '" << pathfile << "'" << std::endl;
		std::cerr << "Exception caught: " << e.what() << std::endl;
		return (1);
	}
	std::cout << "server_rules successfully set.\n";
	return (0);
}

ServerBuilder *build_the_server(const t_server_rules &server_rules, ISocketFactory &socket_factory)
{
	ServerBuilder *new_server_builder;
	try
	{
		new_server_builder = new ServerBuilder (socket_factory);
		new_server_builder->build_server(server_rules);
	}
	catch (const std::exception & e)
	{
		std::cerr << "Building server error." <<std::endl;
		std::cerr << "Exception caught: " << e.what() << std::endl;
		delete new_server_builder;
		throw;
	}
	std::cout << "Server built successfully." << std::endl;
	return new_server_builder;
}

void run_the_server(ServerBuilder *server_builder, const t_server_rules &server_rules)
{
	try
	{
		SessionStorageFramework session_storage;
		SessionPolicyFramework  session_policy(server_rules);
		SessionHandlerUC        session_handler(session_storage, session_policy);

		RequestParser           request_parser;
		RouterHandler           main_handler;
		RequestHandlerUC        request_handler(request_parser, main_handler);

		EpollServerRunner Epoll_server_runner(server_rules, session_handler, request_handler);
		RunnerServer runner_server(server_builder->get_servers(), Epoll_server_runner);

		runner_server.start_the_server();
	}
	catch (const std::exception & e)
	{
		std::cerr << "run the server error." <<std::endl;
		std::cerr << "Exception caught: " << e.what() << std::endl;
	}
}

