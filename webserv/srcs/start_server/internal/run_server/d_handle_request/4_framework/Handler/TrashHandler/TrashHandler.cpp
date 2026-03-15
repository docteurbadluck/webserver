#include "TrashHandler.hpp"

std::string TrashHandler::handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	verify_mandatory_field(req);
	init_field(req, server_rules, session_handler);

	int status_code;
	std::cout << req.methode <<std::endl;

	if (req.methode == "PUT" || req.methode == "PATH" ||req.methode == "OPTIONS" || req.methode == "CONNECT" || req.methode == "TRACE")
		status_code = 501;
	else
		status_code = 400;
	return build_http_response(status_code, "", server_rules.error_page_filepath, "");
}

void TrashHandler::verify_mandatory_field(const t_parsed_request &req)
{
	(void)req;
}

std::string TrashHandler::build_http_response(int status_code, const std::string &body, const std::string &error_page_filepath, const std::string &filepath)
{
	(void)body;
	std::string response_body = load_error_page(error_page_filepath);
	return build_standard_response(status_code, response_body, filepath, false, false);
}
