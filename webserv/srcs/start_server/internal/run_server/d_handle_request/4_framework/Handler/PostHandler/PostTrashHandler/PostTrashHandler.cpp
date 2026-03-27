#include "PostTrashHandler.hpp"

std::string PostTrashHandler::handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	verify_mandatory_field(req);
	init_field(req, server_rules, session_handler);
	int status_code = 405;
	return build_http_response(status_code, request.body, server_rules.error_page_filepath, "");
}

void PostTrashHandler::verify_mandatory_field(const t_parsed_request &req)
{
	if (req.methode != "POST")
		throw std::runtime_error("POST: Invalid method, expected POST");
}

std::string PostTrashHandler::build_http_response(int status_code, const std::string &body, const std::string &error_page_filepath, const std::string &filepath)
{
	(void)body;
	(void)filepath;
	(void)status_code;
	std::string response_body = load_error_page(error_page_filepath);
	return build_standard_response(405, response_body, "", false, false);
}
