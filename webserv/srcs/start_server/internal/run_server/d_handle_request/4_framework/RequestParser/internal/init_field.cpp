#include "RequestParser.hpp"
#include <iostream>

void RequestParser::handle_host(const std::string &value)
{
	parsed_request.host = value;
}

void RequestParser::handle_content_length(const std::string &value)
{
	int					length;
	std::stringstream	ss(value);

	ss >> length;
	parsed_request.body_lenght = length;
}

void RequestParser::handle_content_type(const std::string &value)
{
	parsed_request.content_type = value;
}

void RequestParser::handle_connection(const std::string &value)
{
	std::string	lower_value = value;

	std::transform(lower_value.begin(), lower_value.end(), lower_value.begin(), ::tolower);
	if (lower_value == "close")
		parsed_request.Connection = 1;
	else if (lower_value == "keep-alive")
		parsed_request.Connection = 0;
	else
		throw std::runtime_error("Invalid Connection header value");
}

void RequestParser::handle_cookies(const std::string &value)
{
	size_t		start = 0;
	size_t		separator;
	std::string	cookie;

	while ((separator = value.find(';', start)) != std::string::npos)
	{
		cookie = value.substr(start, separator - start);
		trim(cookie);
		if (!cookie.empty())
			this->parsed_request.cookies.push_back(cookie);
		start = separator + 1;
	}
	handle_last_cookie(value, start);
}

void RequestParser::handle_last_cookie(const std::string &value, const size_t &start)
{
	std::string	cookie;

	if (start < value.size())
	{
		cookie = value.substr(start);
		trim(cookie);
		if (!cookie.empty())
			this->parsed_request.cookies.push_back(cookie);
	}
}