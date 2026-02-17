#include <iostream>
#include "start_server_api.hpp"

int config_is_missing(int argc);

int main(int argc, char **argv)
{
	if (config_is_missing(argc))
        return 1;
    START_SERVER(argv);
	return (0);
}

int config_is_missing(int argc)
{
	if (argc != 2)
	{
		std::cout << "no config given." <<std::endl;
		std::cout << "Usage : ./webserv [config name]" << std::endl;
		return (1);
	}
    return 0;
}
