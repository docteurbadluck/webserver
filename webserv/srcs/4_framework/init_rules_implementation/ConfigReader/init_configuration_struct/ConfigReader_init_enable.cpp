#include "ConfigReader.hpp"

void ConfigReader::init_enable_upload(const std::string& value)
{
	std::istringstream ss(value);
	std::string trimed_value;
	if (ss >> trimed_value && (trimed_value == "true" || trimed_value =="false"))
		this->new_config.enable_upload = (trimed_value == "true");
	else
		throw std::runtime_error("Error config : enable_upload");
}

void ConfigReader::init_enable_directory_listing(const std::string& value)
{
	std::istringstream ss(value);
	std::string trimed_value;

	if (ss >> trimed_value && (trimed_value == "true" || trimed_value =="false"))
		this->new_config.enable_directory_listing = (trimed_value == "true");
	else
		throw std::runtime_error("Error config : enable_directory_listing");
}

void ConfigReader::init_list_allowed_method(const std::string& value)
{
	std::istringstream ss(value);
	std::string method;
	while (ss >> method)
		this->new_config.list_allowed_method.push_back(method);
	if (method.length() == 0)
		throw std::runtime_error("Error config : list_allowed_method");
}

