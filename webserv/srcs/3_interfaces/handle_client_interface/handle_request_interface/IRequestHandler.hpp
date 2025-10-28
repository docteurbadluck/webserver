#pragma once
#include <string>
#include "parsed_request.hpp"
#include "server_rules.hpp"
#include "SessionHandlerUC.hpp"

class IRequestHandler
{
	public :
	virtual std::string handle(const t_parsed_request &req,
								const t_server_rules& server_rules,
								SessionHandlerUC &session_handler) = 0;
	virtual std::vector<std::string>	get_file_content() = 0;
	virtual int							get_is_big_body() = 0;

	virtual ~IRequestHandler() {};
};