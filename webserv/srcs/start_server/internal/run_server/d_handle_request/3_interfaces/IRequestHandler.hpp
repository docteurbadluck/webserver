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
	virtual int							get_fd_stream() = 0;
	virtual pid_t						get_cgi_pid() = 0;
	virtual int							get_close_flag() = 0;
	virtual ~IRequestHandler() {};
};
