#include "PostCloseHandler.hpp"

std::string PostCloseHandler::handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	verify_mandatory_field(req);
	init_field(req, server_rules, session_handler);
	int status_code = set_status_code(req);
	return build_http_response(status_code, request.body, server_rules.error_page_filepath, "");
}

void PostCloseHandler::verify_mandatory_field(const t_parsed_request &req)
{
	if (req.methode != "POST")
		throw std::runtime_error("POST: Invalid method, expected POST");
}

int PostCloseHandler::set_status_code(const t_parsed_request &req)
{
	if (req.body == PASSWORD && req.path == "/close")
		return 200;
	else
		return 403;
}

std::string PostCloseHandler::build_http_response(int status_code, const std::string &body, const std::string &error_page_filepath, const std::string &filepath)
{
	(void)body;
	(void)filepath;
	(void)error_page_filepath;
	std::string response_body;

	if (status_code == 200)
		response_body = "<html><body>Close Server</body></html>";

	return build_standard_response(status_code, response_body, "", false, false);
}
