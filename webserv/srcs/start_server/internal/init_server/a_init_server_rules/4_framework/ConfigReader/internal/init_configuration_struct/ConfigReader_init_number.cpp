#include "ConfigReader.hpp"
#include <netdb.h>

void ConfigReader::init_backlog(const std::string& value)
{
	std::istringstream iss(value);
	iss >> this->new_config.backlog;
	if (iss.fail() || this->new_config.backlog < 1)
		throw std::runtime_error("Error config : backlog");
}

void ConfigReader::init_ip(const std::string& value)
{
	struct addrinfo hints, *res;
	hints = addrinfo();
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_NUMERICHOST;
	int err = getaddrinfo(value.c_str(), NULL, &hints, &res);
	if (err != 0)
		throw std::runtime_error("Error config : ip");
	struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
	this->ip_tmp = ntohl(addr->sin_addr.s_addr);
	freeaddrinfo(res);
}

void ConfigReader::init_port(const std::string& value)
{
	std::istringstream	ss(value);
	std::string			port;
	long				nbr_port;

	ss >> port;
	if (port.empty())
		throw std::runtime_error("Error config: port");
	std::istringstream port_ss(port);
	port_ss >> nbr_port;
	if (port_ss.fail() || !port_ss.eof() || nbr_port < 0 || nbr_port > 65535)
		throw std::runtime_error("Error config: port");
	this->new_config.ip_port_vector.
	push_back(std::make_pair(this->ip_tmp, nbr_port));
}

void ConfigReader::init_max_size_request_body(const std::string& value)
{
	std::istringstream iss(value);
	iss >> this->new_config.max_size_request_body;
	if (iss.fail() || this->new_config.max_size_request_body < 1)
		throw std::runtime_error("Error config : max_size_request_body");
}

