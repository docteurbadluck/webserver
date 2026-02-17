#include "RequestParser.hpp"
#include <iostream>

void RequestParser::parse_first_line(std::string line)
{
		std::istringstream first_line(line);
		first_line >> this->parsed_request.methode;
		first_line >> this->parsed_request.path;
		http_version_initialiser(first_line);
}

void RequestParser::http_version_initialiser(std::istringstream &first_line)
{
	std::string version;
	first_line >> version;
	if (version == "HTTP/1.1")
	{
		this->parsed_request.http_version = 1;
		this->parsed_request.Connection = 0;
	}
	else
	{
		this->parsed_request.http_version = 0;
		this->parsed_request.Connection = 1;
	}
}

void	RequestParser::read_request_line()
{
	std::istringstream	stream(this->request_txt);
	std::string			line;

	std::getline(stream, line);
	if (!line.empty() && line[line.size() - 1] == '\r')
	{
		line.erase(line.size() - 1);
		parse_first_line(line);
	}
	else
		throw std::runtime_error("Invalid HTTP request: empty request line");
}

void	RequestParser::read_header_lines()
{
	std::istringstream	stream(this->request_txt);
	std::string			line;

	std::getline(stream, line);
	while(std::getline(stream, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.empty())
			break;
		attribute_value(line);
	}
}

void	RequestParser::attribute_value(std::string &line)
{
		std::string										name;
		std::string										value;
		size_t											pos;
		std::map<std::string, HeaderHandler>::iterator	it;

		pos = line.find(':');
		if (pos == std::string::npos)
			return ;
		name = line.substr(0, pos);
		value = line.substr(pos + 1);
		trim(name);
		trim(value);
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
		it = header_field_map.find(name);
		if (it != header_field_map.end())
			(this->*(it->second))(value);
}

void	RequestParser::trim(std::string &s)
{
	s.erase(0, s.find_first_not_of(" \t\r\n"));
	s.erase(s.find_last_not_of(" \t\r\n") + 1);
}
