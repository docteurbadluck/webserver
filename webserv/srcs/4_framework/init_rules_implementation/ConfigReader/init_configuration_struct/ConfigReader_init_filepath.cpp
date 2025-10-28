#include "ConfigReader.hpp"

void ConfigReader::init_error_page_filepath(const std::string& value)
{
	std::istringstream	ss(value);
	std::string			error_page_filepath;

	if (ss >> error_page_filepath)
		this->new_config.error_page_filepath = error_page_filepath;
	else 
		throw std::runtime_error("Error config : error_page_filepath");
}

void ConfigReader::init_redirection(const std::string& value)
{
	std::istringstream ss(value);
	std::string from, to;
	if (ss >> from >> to)
		this->new_config.redirection.push_back(std::make_pair(from, to));
	else
	    throw std::runtime_error("Error config : redirection");
}

void ConfigReader::init_file_system_root(const std::string& value)
{
	std::istringstream ss(value);
	std::string uri, path;
	if (ss >> uri >> path)
		this->new_config.file_system_root = std::make_pair(uri, path);
	else
		throw std::runtime_error("Error config : file_system_root");
}

void ConfigReader::init_default_filepath(const std::string& value)
{

	std::istringstream ss(value);
	std::string trimed_value;
	if (ss >> trimed_value)
		this->new_config.default_filepath = trimed_value;
	else
		throw std::runtime_error("Error config : default_filepath");
}

void ConfigReader::init_upload_folder(const std::string& value)
{
	std::istringstream ss(value);
	std::string trimed_value;
	if (ss >> trimed_value)
		this->new_config.upload_folder = trimed_value;
	else
		throw std::runtime_error("Error config : upload_folder");
}

void ConfigReader::init_CGI_pathfile(const std::string& value)
{
	std::istringstream	ss(value);
	std::string			path;

	while (ss >> path)
	{
		this->new_config.CGI_pathfile.push_back(path);
	}
	if (path.length() == 0)
	{
		throw std::runtime_error("Error config : CGI");
	}
}
