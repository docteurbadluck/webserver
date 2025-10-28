#include "ConfigReader.hpp"

void ConfigReader::init_backlog(const std::string& value)
{
	this->new_config.backlog = atoi(value.c_str());
	if (this->new_config.backlog < 1)
		throw std::runtime_error("Error config : backlog");
}

void ConfigReader::init_ip(const std::string& value)
{
	in_addr_t addr = inet_addr(value.c_str());
	if (addr == INADDR_NONE) 
	{
		throw std::runtime_error("Error config : ip");
	}
	this->ip_tmp = ntohl(addr);
}

//then the ip should alway be called first
void ConfigReader::init_port(const std::string& value)
{
	std::istringstream	ss(value);
	std::string			port;
	long				nbr_port;
	char				*endptr;

	ss >> port;
	nbr_port = std::strtol(port.c_str(), &endptr, 10);
	if (nbr_port < 0 || nbr_port > 65535 || port.length() == 0 || *endptr != '\0')
		throw std::runtime_error("Error config: port");
	this->new_config.ip_port_vector.
	push_back(std::make_pair(this->ip_tmp, nbr_port));
}

void ConfigReader::init_max_size_request_body(const std::string& value)
{
	this->new_config.max_size_request_body = atoi(value.c_str());
	if(this->new_config.max_size_request_body < 1)
		throw std::runtime_error("Error config : max_size_request_body");
}

