#include "parsed_request.hpp"
#include <iostream>
#include <iomanip>
#include "framework.hpp"

void print_parsed_request(const t_parsed_request &req)
{
	std::cout << "----- Parsed Request -----\n";
	std::cout << "ID Request: "      << req.id_request << "\n";
	std::cout << "Method: "          << req.methode << "\n";
	std::cout << "Path: "            << req.path << "\n";
	std::cout << "HTTP Version: "    << (req.http_version == 1 ? "HTTP/1.1" : "HTTP/1.0") << "\n";
	std::cout << "Host: "            << req.host << "\n";
	std::cout << "Connection: "      << (req.Connection == 1 ? "close" : "keep-alive") << "\n";
	std::cout << "Body Length: "     << req.body_lenght << "\n";
	std::cout << "Content-Type: "    << req.content_type << "\n";

	std::cout << "Cookies:\n";
	if (req.cookies.empty())
		std::cout << "  (none)\n";
	else
		for (size_t i = 0; i < req.cookies.size(); ++i)
			std::cout << "  - " << req.cookies[i] << "\n";

	std::cout << "Body:\n";
	if (req.body.empty())
		std::cout << "  (empty)\n";
	else
		std::cout << req.body << "\n";
	std::cout << "--------------------------\n";
}