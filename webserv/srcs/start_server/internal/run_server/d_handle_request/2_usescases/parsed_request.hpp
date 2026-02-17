#pragma once
#include <string>
#include <vector>

typedef struct s_parsed_request
{
	int							id_request;
	std::string					session_id;
	std::string					methode;
	std::string					path;
	int							http_version; //1 pour 1.1 0 for 1
	std::string					host;
	int							Connection; //1 close 0 keep alive
	int							body_lenght;
	std::string					content_type;
	std::vector<std::string>	cookies;
	bool						were_cookies_sent;
	std::string					body;
	bool						turn_off_server;
}	t_parsed_request;
