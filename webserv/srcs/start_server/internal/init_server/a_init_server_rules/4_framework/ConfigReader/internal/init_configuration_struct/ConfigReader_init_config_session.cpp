#include "ConfigReader.hpp"


void ConfigReader::init_client_timeout(const std::string& value)
{	
	this->new_config.client_timeout_in_second = atoi(value.c_str());
	if(this->new_config.client_timeout_in_second < 1)
		throw std::runtime_error("Error config : client_timeout");
}

void ConfigReader::init_session_timeout(const std::string& value)
{	
	this->new_config.session_timeout_in_minute = atoi(value.c_str());
	if(this->new_config.session_timeout_in_minute < 1)
		throw std::runtime_error("Error config : session_timeout");
}

void ConfigReader::init_session_secure_only(const std::string &value)
{
	std::istringstream ss(value);
	std::string trimed_value;

	if (ss >> trimed_value && (trimed_value == "true" || trimed_value =="false"))
		this->new_config.session_secure_only = (trimed_value == "true");
	else
	   throw std::runtime_error("Error config : session_secure_only");
}

void ConfigReader::init_session_cookie_name(const std::string &value)
{
	std::istringstream	ss(value);
	std::string			cookie_name;

	if (ss >> cookie_name)
		this->new_config.session_cookie_name = cookie_name;
	else 
		throw std::runtime_error("Error config : session_cookie_name");
}

void ConfigReader::init_max_sessions(const std::string &value)
{
	this->new_config.max_sessions = atoi(value.c_str());
	if(this->new_config.max_sessions < 1)
		throw std::runtime_error("Error config : max_sessions");
}