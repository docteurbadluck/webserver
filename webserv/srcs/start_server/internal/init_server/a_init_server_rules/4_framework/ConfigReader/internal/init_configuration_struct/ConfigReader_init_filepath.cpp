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
		this->new_config.redirection_permanent.push_back(std::make_pair(from, to));
	else
	    throw std::runtime_error("Error config : redirection");
}

void ConfigReader::init_redirection_tmp(const std::string& value)
{
	std::istringstream ss(value);
	std::string from, to;

	if (ss >> from >> to)
		this->new_config.redirection_tmp.push_back(std::make_pair(from, to));
	else
	    throw std::runtime_error("Error config : redirection_tmp");
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


void ConfigReader::init_restrained_pathfile(const std::string& value)
{
    std::istringstream ss(value);
    std::string token;

    while (ss >> token)
    {
        std::string path = token; // premier token = path

        // le token suivant doit être {METHODS}
        if (!(ss >> token) || token.size() < 2 || token[0] != '{' || token[token.size() - 1] != '}')
        {
            throw std::runtime_error("Error config: expected {METHODS}");
        }

        // enlever les accolades
        std::string methods_str = token.substr(1, token.size() - 2);

        // créer un set pour stocker les méthodes
        std::set<std::string> methods;

        std::string::size_type start = 0;
        std::string::size_type comma_pos = methods_str.find(',');

        while (comma_pos != std::string::npos)
        {
            std::string method = methods_str.substr(start, comma_pos - start);
            // enlever espaces avant/après
            while (!method.empty() && method[0] == ' ') method.erase(0,1);
            while (!method.empty() && method[method.size()-1] == ' ') method.erase(method.size()-1,1);

            if (!method.empty())
                methods.insert(method);

            start = comma_pos + 1;
            comma_pos = methods_str.find(',', start);
        }

        // dernier élément après la dernière virgule
        std::string method = methods_str.substr(start);
        while (!method.empty() && method[0] == ' ') method.erase(0,1);
        while (!method.empty() && method[method.size()-1] == ' ') method.erase(method.size()-1,1);
        if (!method.empty())
            methods.insert(method);

        // stocker dans la map
        this->new_config.restrained_filepath[path] = methods;
    }

    if (this->new_config.restrained_filepath.empty())
    {
        throw std::runtime_error("Error config: no restrained paths found");
    }
}

